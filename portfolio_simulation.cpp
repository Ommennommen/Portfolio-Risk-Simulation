#include <bits/stdc++.h>
using namespace std;

/*
 * Portfolio Risk Simulation
 * -------------------------
 * Reads a CSV file of historical daily returns for multiple assets,
 * runs a Monte Carlo simulation to generate random portfolios,
 * and outputs their return, volatility, Sharpe ratio, and weights to a CSV.
 *
 * USAGE:
 *   ./portfolio_sim returns.csv N rf annualize
 *     returns.csv - CSV file with columns: date,asset1,asset2,...
 *     N           - Number of random portfolios to simulate
 *     rf          - Risk-free rate (annual, e.g., 0.02 for 2%)
 *     annualize   - 1 to annualize return/volatility, 0 to keep daily
 *
 * OUTPUT:
 *   portfolios.csv - CSV with sharpe,ret,vol, and weights per asset
 *
 * NOTE:
 *   - No short selling (weights >= 0, sum to 1)
 *   - If annualized, assumes 252 trading days per year
 */

struct Data {
    vector<string> tickers;            // Asset names from CSV header
    vector<vector<double>> returns;    // T x K matrix of daily returns
};

/**
 * Reads returns CSV into memory.
 * Expected format: date,asset1,asset2,...
 */
Data read_returns_csv(const string& path) {
    ifstream f(path);
    string line;
    Data d;
    vector<vector<double>> rows;

    if (!getline(f, line))
        throw runtime_error("Empty CSV file.");

    // Parse header row
    stringstream hs(line);
    string cell;
    vector<string> header;
    while (getline(hs, cell, ',')) header.push_back(cell);

    // Asset tickers are everything after the date column
    d.tickers.assign(header.begin() + 1, header.end());

    // Parse return rows
    while (getline(f, line)) {
        stringstream ss(line);
        vector<double> row;
        row.reserve(d.tickers.size());

        string c;
        bool first = true;
        while (getline(ss, c, ',')) {
            if (first) { first = false; continue; } // skip date column
            if (c.empty()) row.push_back(0.0);
            else row.push_back(stod(c));
        }
        if (!row.empty()) rows.push_back(row);
    }

    d.returns = move(rows);
    return d;
}

/**
 * Computes mean returns for each asset.
 */
vector<double> mean_vec(const vector<vector<double>>& R) {
    size_t T = R.size(), K = R[0].size();
    vector<double> mu(K, 0.0);

    for (size_t t = 0; t < T; ++t)
        for (size_t k = 0; k < K; ++k)
            mu[k] += R[t][k];

    for (size_t k = 0; k < K; ++k)
        mu[k] /= static_cast<double>(T);

    return mu;
}

/**
 * Computes sample covariance matrix for asset returns.
 */
vector<vector<double>> cov_mat(const vector<vector<double>>& R, const vector<double>& mu) {
    size_t T = R.size(), K = R[0].size();
    vector<vector<double>> S(K, vector<double>(K, 0.0));

    for (size_t t = 0; t < T; ++t) {
        for (size_t i = 0; i < K; ++i) {
            double xi = R[t][i] - mu[i];
            for (size_t j = i; j < K; ++j) {
                double xj = R[t][j] - mu[j];
                S[i][j] += xi * xj;
            }
        }
    }

    double denom = static_cast<double>(T - 1);
    for (size_t i = 0; i < K; ++i) {
        for (size_t j = i; j < K; ++j) {
            S[i][j] /= denom;
            S[j][i] = S[i][j]; // symmetry
        }
    }
    return S;
}

/**
 * Generates random portfolio weights (sum to 1, no short selling).
 */
vector<double> random_weights(size_t K, mt19937& rng) {
    uniform_real_distribution<double> U(0.0, 1.0);
    vector<double> w(K);
    double sum = 0.0;

    for (size_t k = 0; k < K; ++k) {
        w[k] = U(rng) + 1e-9; // avoid zero
        sum += w[k];
    }
    for (size_t k = 0; k < K; ++k) w[k] /= sum;

    return w;
}

/**
 * Computes dot product between two vectors.
 */
double dot(const vector<double>& a, const vector<double>& b) {
    double s = 0.0;
    for (size_t i = 0; i < a.size(); ++i) s += a[i] * b[i];
    return s;
}

/**
 * Computes w' S w for portfolio variance.
 */
double quad(const vector<double>& w, const vector<vector<double>>& S) {
    size_t K = w.size();
    double s = 0.0;

    for (size_t i = 0; i < K; ++i) {
        double tmp = 0.0;
        for (size_t j = 0; j < K; ++j)
            tmp += S[i][j] * w[j];
        s += w[i] * tmp;
    }
    return s;
}

int main(int argc, char** argv) {
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " returns.csv N rf annualize(0/1)\n";
        return 1;
    }

    string path = argv[1];
    int N = stoi(argv[2]);             // number of portfolios to simulate
    double rf = stod(argv[3]);         // annual risk-free rate
    bool annualize = stoi(argv[4]) != 0; // annualize results?

    // Load returns data
    auto D = read_returns_csv(path);
    if (D.returns.empty()) {
        cerr << "No data in returns file.\n";
        return 1;
    }

    // Mean & covariance (daily)
    auto mu = mean_vec(D.returns);
    auto S  = cov_mat(D.returns, mu);

    // Annualization factors
    const double days = 252.0;
    vector<double> mu_ann = mu;
    vector<vector<double>> S_ann = S;
    if (annualize) {
        for (auto& m : mu_ann) m *= days;
        for (size_t i = 0; i < S.size(); ++i)
            for (size_t j = 0; j < S.size(); ++j)
                S_ann[i][j] *= days; // variance scales linearly with time
    }

    // Monte Carlo simulation
    mt19937 rng(42);
    ofstream out("portfolios.csv");
    out << "sharpe,ret,vol";
    for (auto& t : D.tickers) out << "," << t;
    out << "\n";

    for (int n = 0; n < N; ++n) {
        auto w = random_weights(D.tickers.size(), rng);

        double ret = dot(w, annualize ? mu_ann : mu);
        double var = quad(w, annualize ? S_ann : S);
        double vol = sqrt(max(var, 0.0));
        double sharpe = (annualize ? (ret - rf) : ret) / (vol + 1e-12);

        out << sharpe << "," << ret << "," << vol;
        for (auto wi : w) out << "," << wi;
        out << "\n";
    }

    cerr << "✅ Wrote portfolios.csv (" << N << " portfolios simulated)\n";
    return 0;
}
