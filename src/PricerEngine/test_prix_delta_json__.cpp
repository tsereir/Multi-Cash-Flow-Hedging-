//#include <iostream>
//#include <fstream>
//#include "nlohmann/json.hpp"
//#include "json_reader.hpp"
//#include "pricer.hpp"
//
//using json = nlohmann::json;
//using namespace std;
//
//int main() {
//    // Liste des chemins vers les fichiers JSON de paramètres
//    vector<string> jsonFiles = {
//            "/home/ensimag/Tariq COURS 3A TARIQ/Methodes de Monte Carlo/TP6/tp6/math-param1.json",
//            "/home/ensimag/Tariq COURS 3A TARIQ/Methodes de Monte Carlo/TP6/tp6/math-param2.json",
//            "/home/ensimag/Tariq COURS 3A TARIQ/Methodes de Monte Carlo/TP6/tp6/math-param3.json",
//            "/home/ensimag/Tariq COURS 3A TARIQ/Methodes de Monte Carlo/TP6/tp6/math-param4.json"
//    };
//
//    // Parcourir chaque fichier JSON
//    for (const auto &filePath : jsonFiles) {
//        cout << "Processing: " << filePath << endl;
//        ifstream ifs(filePath);
//        if (!ifs.is_open()) {
//            cerr << "Could not open file: " << filePath << endl;
//            continue;
//        }
//        json j;
//        try {
//            ifs >> j;
//        } catch (json::parse_error& e) {
//            cerr << "Error parsing JSON: " << e.what() << endl;
//            continue;
//        }
//        BlackScholesPricer pricer(j);
//
//        PnlMat* pastData = pnl_mat_create_from_list(1, pricer.nAssets, 10.3,10.3,10.3,10.3);  // 100 est un exemple pour le prix initial
//        double computedPrice;
//        double priceVariance;
//        PnlVect* assetDeltas;
//        PnlVect* deltaVariances;
//
//        // Calculer le prix et les deltas en t=0
//        pricer.priceAndDeltas(pastData, 0.0, false, computedPrice, priceVariance, assetDeltas, deltaVariances);
//        cout << "Computed Price: " << computedPrice << endl;
//        cout << "Price Variance: " << priceVariance << endl;
//        cout << "Deltas: ";
//        pnl_vect_print_asrow(assetDeltas);
//
//        // Libération des ressources
//        pnl_mat_free(&pastData);
//        pnl_vect_free(&assetDeltas);
//        pnl_vect_free(&deltaVariances);
//
//        cout << "-----------------------------------" << endl;
//    }
//
//    return 0;
//}
