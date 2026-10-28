#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <random>
#include <iomanip>

struct Noeud {
    int valeur;
    Noeud* gauche;
    Noeud* droit;

    explicit Noeud(int v) : valeur(v), gauche(nullptr), droit(nullptr) {}
};

// Insere recursivement une valeur. Les doublons sont ignores.
Noeud* inserer(Noeud* racine, int valeur, bool& inseree) {
    if (racine == nullptr) {
        inseree = true;
        return new Noeud(valeur);
    }

    if (valeur < racine->valeur)
        racine->gauche = inserer(racine->gauche, valeur, inseree);
    else if (valeur > racine->valeur)
        racine->droit = inserer(racine->droit, valeur, inseree);

    return racine;
}

bool rechercher(const Noeud* racine, int valeur) {
    if (racine == nullptr) return false;
    if (valeur == racine->valeur) return true;

    if (valeur < racine->valeur)
        return rechercher(racine->gauche, valeur);
    return rechercher(racine->droit, valeur);
}

Noeud* minimum(Noeud* noeud) {
    while (noeud != nullptr && noeud->gauche != nullptr)
        noeud = noeud->gauche;
    return noeud;
}

Noeud* supprimer(Noeud* racine, int valeur, bool& supprimee) {
    if (racine == nullptr) return nullptr;

    if (valeur < racine->valeur) {
        racine->gauche = supprimer(racine->gauche, valeur, supprimee);
    } else if (valeur > racine->valeur) {
        racine->droit = supprimer(racine->droit, valeur, supprimee);
    } else {
        supprimee = true;

        // Aucun enfant ou un seul enfant.
        if (racine->gauche == nullptr) {
            Noeud* enfant = racine->droit;
            delete racine;
            return enfant;
        }
        if (racine->droit == nullptr) {
            Noeud* enfant = racine->gauche;
            delete racine;
            return enfant;
        }

        // Deux enfants : remplacement par le successeur en ordre croissant.
        Noeud* successeur = minimum(racine->droit);
        racine->valeur = successeur->valeur;
        bool successeurSupprime = false;
        racine->droit = supprimer(racine->droit, successeur->valeur,
                                  successeurSupprime);
    }

    return racine;
}

void afficherInfixe(const Noeud* racine) {
    if (racine == nullptr) return;
    afficherInfixe(racine->gauche);
    std::cout << racine->valeur << ' ';
    afficherInfixe(racine->droit);
}

void libererArbre(Noeud* racine) {
    if (racine == nullptr) return;
    libererArbre(racine->gauche);
    libererArbre(racine->droit);
    delete racine;
}

int main() {
    Noeud* racine = nullptr;
    int nombreOperations;
    std::cout << "Nombre de valeurs a inserer, rechercher et supprimer : ";
    if (!(std::cin >> nombreOperations) || nombreOperations < 0) {
        std::cerr << "Nombre invalide.\n";
        return 1;
    }

    // Une permutation aleatoire garantit des valeurs d'insertion uniques.
    std::vector<int> valeursInsertion(nombreOperations);
    std::iota(valeursInsertion.begin(), valeursInsertion.end(), 1);
    std::random_device graine;
    std::mt19937 generateur(graine());
    std::shuffle(valeursInsertion.begin(), valeursInsertion.end(), generateur);

    // Recherche et suppression utilisent aussi des ordres aleatoires.
    std::vector<int> valeursRecherche = valeursInsertion;
    std::vector<int> valeursSuppression = valeursInsertion;
    std::shuffle(valeursRecherche.begin(), valeursRecherche.end(), generateur);
    std::shuffle(valeursSuppression.begin(), valeursSuppression.end(), generateur);

    using Horloge = std::chrono::high_resolution_clock;
    using Nanosecondes = std::chrono::duration<double, std::nano>;

    auto debutInsertion = Horloge::now();
    int nombreInserees = 0;
    for (int valeur : valeursInsertion) {
        bool inseree = false;
        racine = inserer(racine, valeur, inseree);
        if (inseree) ++nombreInserees;
    }
    auto finInsertion = Horloge::now();

    auto debutRecherche = Horloge::now();
    int nombreTrouvees = 0;
    for (int valeur : valeursRecherche)
        if (rechercher(racine, valeur)) ++nombreTrouvees;
    auto finRecherche = Horloge::now();

    auto debutSuppression = Horloge::now();
    int nombreSupprimees = 0;
    for (int valeur : valeursSuppression) {
        bool supprimee = false;
        racine = supprimer(racine, valeur, supprimee);
        if (supprimee) ++nombreSupprimees;
    }
    auto finSuppression = Horloge::now();

    const double dureeInsertion = Nanosecondes(finInsertion - debutInsertion).count() / 1e9;
    const double dureeRecherche = Nanosecondes(finRecherche - debutRecherche).count() / 1e9;
    const double dureeSuppression = Nanosecondes(finSuppression - debutSuppression).count() / 1e9;

    std::cout << std::setprecision(9) << std::defaultfloat;
    std::cout << "\nResultats :\n"
              << "Valeurs generees aleatoirement (" << nombreOperations << " valeurs uniques).\n"
              << "Insertions reussies : " << nombreInserees << "/" << nombreOperations << '\n'
              << "Duree totale des insertions : " << dureeInsertion << " secondes\n"
              << "Recherches trouvees : " << nombreTrouvees << "/" << nombreOperations << '\n'
              << "Duree totale des recherches : " << dureeRecherche << " secondes\n"
              << "Suppressions reussies : " << nombreSupprimees << "/" << nombreOperations << '\n'
              << "Duree totale des suppressions : " << dureeSuppression << " secondes\n";

    std::cout << "Arbre restant en ordre croissant : ";
    afficherInfixe(racine);
    std::cout << '\n';

    libererArbre(racine);
    return 0;
}
