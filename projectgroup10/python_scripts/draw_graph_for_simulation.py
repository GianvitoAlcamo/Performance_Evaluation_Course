import os
import glob
import re
import matplotlib.pyplot as plt
from collections import defaultdict


def clean_stat_name(stat):
    """
    Prende il nome originale della statistica e restituisce
    solo la parte prima di 'Stat'.
    """
    if "Stat" in stat:
        return stat.split("Stat")[0]
    else:
        return stat


# ==========================
# PARAMETRI
# ==========================

DATA_DIR = r"C:\Users\alcam\OneDrive\Desktop\dati\csv"
N_TESTS = 50

print("CSV trovati:", len(glob.glob(os.path.join(DATA_DIR, "*.csv"))))

# Nomi delle statistiche da cercare nel CSV
STATISTICS = [
    "utilizationStat:timeavg,,,",
    "responseTimeStat:mean,,,",
    "responseTimeContStat:mean,,,",
    "responseTimeFaasStat:mean,,,"
]

# ==========================
# REGEX NOME FILE
# ==========================
filename_pattern = re.compile(
    r"Simulation-Test-(?:\d+-)?(\d+),(\d+),([\d\.]+)-#(\d+)\.csv"
)

# ==========================
# STRUTTURA DATI
# ==========================
# (stat, p3, p1, p2) -> [valori delle 50 ripetizioni]
raw_data = defaultdict(list)

# ==========================
# LETTURA FILE
# ==========================
for filepath in glob.glob(os.path.join(DATA_DIR, "*.csv")):
    filename = os.path.basename(filepath)
    match = filename_pattern.match(filename)
    if not match:
        continue

    p1 = float(match.group(1))
    p2 = int(match.group(2))
    p3 = float(match.group(3))

    with open(filepath, "r") as f:
        lines = f.readlines()

    for line in lines:
        for stat in STATISTICS:
            if stat in line:
                # prendo l'ultimo campo dopo la virgola
                try:
                    value = float(line.strip().split(",")[-1])
                    raw_data[(stat, p3, p1, p2)].append(value)
                except ValueError:
                    pass

# ==========================
# MEDIA SULLE 50 RIPETIZIONI
# ==========================
averaged_data = {}

for key, values in raw_data.items():
    if len(values) == N_TESTS:
        averaged_data[key] = sum(values) / len(values)
    else:
        print("ATTENZIONE:", key, "→", len(values), "file trovati")

# ==========================
# CREAZIONE GRAFICI
# ==========================

GRAPH_DIR = os.path.join(DATA_DIR, "grafici")
os.makedirs(GRAPH_DIR, exist_ok=True)

statistics = STATISTICS
p3_values = sorted(set(k[1] for k in averaged_data))

# Creazione grafici con segmenti e salvataggio
for stat in statistics:
    for p3 in p3_values:
        plt.figure(figsize=(8,6))

        p2_groups = defaultdict(list)
        for (stat_name, p3_val, p1, p2), value in averaged_data.items():
            if stat_name == stat and p3_val == p3:
                p2_groups[p2].append((p1, value))

        for p2, points in p2_groups.items():
            points.sort()  # ordina per p1
            x = [p[0] for p in points]
            y = [p[1] for p in points]
            plt.plot(x, y, marker='o', label=f"k={p2}")

        clean_name = clean_stat_name(stat)
        plt.xlabel("Number of Users")
        plt.ylabel(clean_name)
        plt.title(f"{clean_name} — P = {p3}")
        
        if clean_name.lower() != "utilization":
            plt.yscale("log")
        
        plt.legend()
        plt.grid(True, which="both")

        # Salva il grafico
        filename_safe = clean_name
        filepath_save = os.path.join(GRAPH_DIR, f"{filename_safe}_p3_{p3}.png")
        plt.savefig(filepath_save, dpi=300)
        plt.close()




# Shell Script used to generate the CSV files

#for FILE in Simulation-Test-*.sca
#do
#    name=$(basename "$FILE" .sca)
#    /home/opp_env/default_workspace/omnetpp-6.2.0/bin/opp_scavetool export --type s -o "$name.csv" "$FILE"
#done
