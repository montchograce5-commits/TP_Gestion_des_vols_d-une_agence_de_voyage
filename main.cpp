#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <cstdlib>

using namespace std;

// Saisies securisees : getline evite les problemes de buffer de cin.
string lireTexte(const string& invite) {
    string s;
    while (true) {
        cout << invite;
        if (!getline(cin, s)) {
            cout << "\nFin de saisie. Arret du programme.\n";
            exit(0);
        }
        if (s.find_first_not_of(" \t\r\n") != string::npos) return s;
        cout << "Erreur : la valeur ne peut pas etre vide.\n";
    }
}

int lireEntier(const string& invite, int minimum = 0) {
    string s;
    int n;
    char reste;
    while (true) {
        cout << invite;
        if (!getline(cin, s)) exit(0);
        stringstream flux(s);
        if ((flux >> n) && !(flux >> reste) && n >= minimum) return n;
        cout << "Erreur : entrez un entier valide (minimum " << minimum << ").\n";
    }
}

double lireMontant(const string& invite) {
    string s;
    double n;
    char reste;
    while (true) {
        cout << invite;
        if (!getline(cin, s)) exit(0);
        replace(s.begin(), s.end(), ',', '.');
        stringstream flux(s);
        if ((flux >> n) && !(flux >> reste) && n > 0 &&
            n <= numeric_limits<double>::max()) return n;
        cout << "Erreur : entrez un montant strictement positif.\n";
    }
}

bool emailValide(const string& e) {
    size_t a = e.find('@');
    return a != string::npos && a > 0 &&
           e.find('.', a + 2) != string::npos &&
           e.find('.', a + 2) < e.size() - 1;
}

string lireEmail() {
    while (true) {
        string e = lireTexte("Email : ");
        if (emailValide(e)) return e;
        cout << "Erreur : format d'email invalide.\n";
    }
}

bool dateValide(const string& d) {
    if (d.size() != 10 || d[2] != '/' || d[5] != '/') return false;
    for (int i : {0, 1, 3, 4, 6, 7, 8, 9})
        if (!isdigit(static_cast<unsigned char>(d[i]))) return false;

    int jour = stoi(d.substr(0, 2));
    int mois = stoi(d.substr(3, 2));
    int annee = stoi(d.substr(6, 4));
    if (annee < 1 || mois < 1 || mois > 12 || jour < 1) return false;

    const int jours[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int maxJour = jours[mois - 1];
    bool bissextile = annee % 400 == 0 || (annee % 4 == 0 && annee % 100 != 0);
    if (mois == 2 && bissextile) maxJour = 29;
    return jour <= maxJour;
}

string lireDate() {
    while (true) {
        string d = lireTexte("Date (JJ/MM/AAAA) : ");
        if (dateValide(d)) return d;
        cout << "Erreur : date invalide. Exemple : 07/04/2027.\n";
    }
}

class Passager {
    int id;
    string nom, prenom, email, telephone;
public:
    Passager(int i, string n, string p, string e, string t)
        : id(i), nom(n), prenom(p), email(e), telephone(t) {}
    int getId() const { return id; }
    void afficher() const {
        cout << "Passager #" << id << " : " << prenom << " " << nom
             << " | " << email << " | " << telephone << '\n';
    }
};

class Vol {
    int id, capacite, placesReservees;
    string numero, depart, destination, date;
    double prix;
public:
    Vol(int i, string num, string dep, string dest, string d, int cap, double p)
        : id(i), capacite(cap), placesReservees(0), numero(num),
          depart(dep), destination(dest), date(d), prix(p) {}
    int getId() const { return id; }
    double getPrix() const { return prix; }
    bool estDisponible() const { return placesReservees < capacite; }
    bool correspondDestination(const string& recherche) const {
        auto normaliser = [](string s) {
            transform(s.begin(), s.end(), s.begin(),
                [](unsigned char c) { return static_cast<char>(tolower(c)); });
            return s;
        };
        return estDisponible() &&
            normaliser(destination).find(normaliser(recherche)) != string::npos;
    }
    bool reserverPlace() {
        if (!estDisponible()) return false;
        ++placesReservees;
        return true;
    }
    void libererPlace() {
        if (placesReservees > 0) --placesReservees;
    }
    void afficher() const {
        cout << "Vol #" << id << " | " << numero << " | " << depart << " -> "
             << destination << " | " << date << " | Places libres : "
             << capacite - placesReservees << "/" << capacite
             << " | Prix : " << fixed << setprecision(2) << prix << '\n';
    }
};

enum class EtatReservation { EN_ATTENTE, VALIDEE, ANNULEE };

string nomEtat(EtatReservation e) {
    switch (e) {
        case EtatReservation::EN_ATTENTE: return "En attente";
        case EtatReservation::VALIDEE: return "Validee";
        case EtatReservation::ANNULEE: return "Annulee";
    }
    return "Inconnu";
}

class Reservation {
    int id, idPassager, idVol;
    string dateCreation;
    EtatReservation etat;
public:
    Reservation(int i, int p, int v, string d)
        : id(i), idPassager(p), idVol(v), dateCreation(d),
          etat(EtatReservation::EN_ATTENTE) {}
    int getId() const { return id; }
    int getIdPassager() const { return idPassager; }
    int getIdVol() const { return idVol; }
    EtatReservation getEtat() const { return etat; }
    void setEtat(EtatReservation e) { etat = e; }
    void afficher() const {
        cout << "Reservation #" << id << " | Passager #" << idPassager
             << " | Vol #" << idVol << " | " << dateCreation
             << " | " << nomEtat(etat) << '\n';
    }
};

enum class EtatPaiement { PAYE, REMBOURSE_EN_ATTENTE, REMBOURSE };

string nomPaiement(EtatPaiement e) {
    switch (e) {
        case EtatPaiement::PAYE: return "Paye";
        case EtatPaiement::REMBOURSE_EN_ATTENTE: return "Remboursement en attente";
        case EtatPaiement::REMBOURSE: return "Rembourse";
    }
    return "Inconnu";
}

class Paiement {
    int id, idReservation;
    double montant;
    EtatPaiement etat;
public:
    Paiement(int i, int r, double m)
        : id(i), idReservation(r), montant(m), etat(EtatPaiement::PAYE) {}
    int getIdReservation() const { return idReservation; }
    EtatPaiement getEtat() const { return etat; }
    void demanderRemboursement() {
        if (etat == EtatPaiement::PAYE) etat = EtatPaiement::REMBOURSE_EN_ATTENTE;
    }
    void confirmerRemboursement() {
        if (etat == EtatPaiement::REMBOURSE_EN_ATTENTE) etat = EtatPaiement::REMBOURSE;
    }
    void afficher() const {
        cout << "Paiement #" << id << " | Reservation #" << idReservation
             << " | " << fixed << setprecision(2) << montant
             << " | " << nomPaiement(etat) << '\n';
    }
};

class AgenceVoyage {
    vector<Passager> passagers;
    vector<Vol> vols;
    vector<Reservation> reservations;
    vector<Paiement> paiements;
    int prochainPassager = 1, prochainVol = 1;
    int prochaineReservation = 1, prochainPaiement = 1;

    Passager* passager(int id) {
        for (auto& p : passagers) if (p.getId() == id) return &p;
        return nullptr;
    }
    Vol* vol(int id) {
        for (auto& v : vols) if (v.getId() == id) return &v;
        return nullptr;
    }
    Reservation* reservation(int id) {
        for (auto& r : reservations) if (r.getId() == id) return &r;
        return nullptr;
    }
    Paiement* paiementPour(int idReservation) {
        for (auto& p : paiements)
            if (p.getIdReservation() == idReservation) return &p;
        return nullptr;
    }

public:
    void enregistrerPassager() {
        cout << "\n--- Enregistrement passager ---\n";
        string n = lireTexte("Nom : ");
        string p = lireTexte("Prenom : ");
        string e = lireEmail();
        string t = lireTexte("Telephone : ");
        passagers.emplace_back(prochainPassager++, n, p, e, t);
        cout << "Passager enregistre.\n";
    }

    void ajouterVol() {
        cout << "\n--- Ajout vol ---\n";
        string num = lireTexte("Numero du vol : ");
        string dep = lireTexte("Ville de depart : ");
        string dest = lireTexte("Destination : ");
        string d = lireDate();
        int cap = lireEntier("Capacite : ", 1);
        double p = lireMontant("Prix : ");
        vols.emplace_back(prochainVol++, num, dep, dest, d, cap, p);
        cout << "Vol ajoute.\n";
    }

    void afficherVols() const {
        cout << "\n--- Vols disponibles ---\n";
        bool trouve = false;
        for (const auto& v : vols) if (v.estDisponible()) {
            v.afficher(); trouve = true;
        }
        if (!trouve) cout << "Aucun vol disponible.\n";
    }

    void rechercherVol() const {
        string dest = lireTexte("Destination recherchee : ");
        bool trouve = false;
        for (const auto& v : vols) if (v.correspondDestination(dest)) {
            v.afficher(); trouve = true;
        }
        if (!trouve) cout << "Aucun vol disponible pour cette destination.\n";
    }

    void effectuerReservation() {
        if (passagers.empty() || vols.empty()) {
            cout << "Erreur : ajoutez au moins un passager et un vol avant de reserver.\n";
            return;
        }
        int idP = lireEntier("ID du passager : ", 1);
        if (!passager(idP)) { cout << "Erreur : passager introuvable.\n"; return; }
        int idV = lireEntier("ID du vol : ", 1);
        if (!vol(idV)) { cout << "Erreur : vol introuvable.\n"; return; }
        reservations.emplace_back(prochaineReservation++, idP, idV, lireDate());
        cout << "Demande de reservation creee : statut En attente.\n";
    }

    void validerReservation() {
        int id = lireEntier("ID de la reservation : ", 1);
        Reservation* r = reservation(id);
        if (!r) { cout << "Erreur : reservation introuvable.\n"; return; }
        if (r->getEtat() != EtatReservation::EN_ATTENTE) {
            cout << "Erreur : seule une reservation en attente peut etre validee.\n";
            return;
        }
        Vol* v = vol(r->getIdVol());
        if (!v || !v->reserverPlace()) {
            cout << "Erreur : aucune place disponible sur ce vol.\n"; return;
        }
        r->setEtat(EtatReservation::VALIDEE);
        cout << "Reservation validee. Place attribuee.\n";
    }

    void enregistrerPaiement() {
        int id = lireEntier("ID de la reservation : ", 1);
        Reservation* r = reservation(id);
        if (!r) { cout << "Erreur : reservation introuvable.\n"; return; }
        if (r->getEtat() != EtatReservation::VALIDEE) {
            cout << "Erreur : la reservation doit etre validee avant le paiement.\n";
            return;
        }
        if (paiementPour(id)) {
            cout << "Erreur : un paiement existe deja pour cette reservation.\n";
            return;
        }
        Vol* v = vol(r->getIdVol());
        if (!v) { cout << "Erreur : vol associe introuvable.\n"; return; }
        paiements.emplace_back(prochainPaiement++, id, v->getPrix());
        cout << "Paiement enregistre (simulation uniquement).\n";
    }

    void annulerReservation() {
        int id = lireEntier("ID de la reservation : ", 1);
        Reservation* r = reservation(id);
        if (!r) { cout << "Erreur : reservation introuvable.\n"; return; }
        if (r->getEtat() == EtatReservation::ANNULEE) {
            cout << "Erreur : reservation deja annulee.\n"; return;
        }
        if (r->getEtat() == EtatReservation::VALIDEE) {
            Vol* v = vol(r->getIdVol());
            if (v) v->libererPlace();
        }
        r->setEtat(EtatReservation::ANNULEE);
        Paiement* p = paiementPour(id);
        if (p && p->getEtat() == EtatPaiement::PAYE) {
            p->demanderRemboursement();
            cout << "Reservation annulee. Remboursement mis en attente.\n";
        } else cout << "Reservation annulee.\n";
    }

    void consulterReservations() const {
        if (reservations.empty()) { cout << "Aucune reservation.\n"; return; }
        for (const auto& r : reservations) r.afficher();
    }

    void informationsPassager() const {
        int id = lireEntier("ID du passager : ", 1);
        auto it = find_if(passagers.begin(), passagers.end(),
            [id](const Passager& p) { return p.getId() == id; });
        if (it == passagers.end()) { cout << "Erreur : passager introuvable.\n"; return; }
        it->afficher();
        bool trouve = false;
        for (const auto& r : reservations) if (r.getIdPassager() == id) {
            r.afficher(); trouve = true;
        }
        if (!trouve) cout << "Aucune reservation pour ce passager.\n";
    }

    void traiterRemboursement() {
        int id = lireEntier("ID de la reservation : ", 1);
        Paiement* p = paiementPour(id);
        if (!p) { cout << "Erreur : aucun paiement associe.\n"; return; }
        if (p->getEtat() != EtatPaiement::REMBOURSE_EN_ATTENTE) {
            cout << "Erreur : aucun remboursement en attente.\n"; return;
        }
        p->confirmerRemboursement();
        cout << "Remboursement confirme (simulation uniquement).\n";
    }

    void consulterPaiements() const {
        if (paiements.empty()) { cout << "Aucun paiement.\n"; return; }
        for (const auto& p : paiements) p.afficher();
    }
};

int main() {
    AgenceVoyage agence;
    int choix;
    do {
        cout << "\n========== AGENCE DE VOYAGES ==========\n"
             << "1. Enregistrer un passager\n"
             << "2. Ajouter un vol\n"
             << "3. Afficher les vols disponibles\n"
             << "4. Rechercher un vol par destination\n"
             << "5. Effectuer une reservation\n"
             << "6. Valider une reservation\n"
             << "7. Enregistrer un paiement\n"
             << "8. Annuler une reservation\n"
             << "9. Consulter les reservations\n"
             << "10. Informations d'un passager\n"
             << "11. Traiter un remboursement\n"
             << "12. Consulter les paiements\n"
             << "0. Quitter\n";
        choix = lireEntier("Votre choix : ", 0);
        switch (choix) {
            case 1: agence.enregistrerPassager(); break;
            case 2: agence.ajouterVol(); break;
            case 3: agence.afficherVols(); break;
            case 4: agence.rechercherVol(); break;
            case 5: agence.effectuerReservation(); break;
            case 6: agence.validerReservation(); break;
            case 7: agence.enregistrerPaiement(); break;
            case 8: agence.annulerReservation(); break;
            case 9: agence.consulterReservations(); break;
            case 10: agence.informationsPassager(); break;
            case 11: agence.traiterRemboursement(); break;
            case 12: agence.consulterPaiements(); break;
            case 0: cout << "Fermeture de l'application.\n"; break;
            default: cout << "Choix invalide.\n";
        }
    } while (choix != 0);
    return 0;
}
