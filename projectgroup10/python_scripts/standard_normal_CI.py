import pandas as pd
import numpy as np
import scipy.stats as st
import matplotlib.pyplot as plt
import sys

# --- CONFIGURATION ---
# We insert the correct input file path
# The desired performance metric
# And the reference Theoretical Value
FILENAME = '../theoretical_test_02/record_csv/50_runs_2h_01.csv'
METRIC_NAME = 'responseTimeStat:mean' # La metrica target
THEORETICAL_VALUE = 5.07574       # Il valore Erlang-C calcolato teoricamente

def analyze_sample():
    print("--- STEP 1: ANALYZING THE SAMPLE (N=50) ---\n")

    # 1. LOADING THE DATA
    # Loading the simulation result CSV file taken from OMNeT++
    try:
        df = pd.read_csv(FILENAME)
    except FileNotFoundError:
        print(f"ERROR!: file {FILENAME} doesn't exist.")
        return

    # 2. FILTERING
    # We only want scalar values for 'responseTimeStat:mean'
    df_filtered = df[(df['name'] == METRIC_NAME) & (df['type'] == 'scalar')]
    values = pd.to_numeric(df_filtered['value'], errors='coerce').dropna().values

    # We check if the data are present
    if len(values) == 0:
        print("ERROR! Data not found. Check metric name.")
        return

    # 3. STATISTICS
    n = len(values)
    mean_val = np.mean(values)
    std_val = np.std(values, ddof=1) # ddof=1 for sample standard deviation (unbiased)
    se_val = std_val / np.sqrt(n)    # standard error for the mean

    print(f"Data loaded: {n} runs.")
    print(f"Sample Mean: {mean_val:.5f} s")
    print(f"Std Deviation: {std_val:.5f} s")

    # 4. CONFIDENCE INTERVAL COMPUTATION (GAUSSIANA)
    # If we have N >= 30 we can use our sample mean in a Normal Distribution
    # basically we can approximate the sample mean distribution to a Normal one (Z).
    # Confidence Level 95% -> Z alpha/2 = 1.96
    confidence_level = 0.95
    z_score = st.norm.ppf((1 + confidence_level) / 2)
    
    margin_of_error = z_score * se_val
    ci_lower = mean_val - margin_of_error
    ci_upper = mean_val + margin_of_error

    print(f"\n--- INTERVAL ANALYSIS (GAUSSIAN) ---")
    print(f"95% interval: [{ci_lower:.5f}, {ci_upper:.5f}]")
    print(f"Theoretical Value: {THEORETICAL_VALUE:.5f}")
    
    # We verify that the theoretical value stays in the confidence interval
    inside = (ci_lower <= THEORETICAL_VALUE <= ci_upper)
    if inside:
        print("RESULT: THE VALUE IS IN (PASSED)")
    else:
        print("RESULT: THE VALUE IS OUT (FAILED)")
        print("WARNING: Check the distance between the theoretical value and the confidence interval and verify data normality distribution")

if __name__ == "__main__":
    analyze_sample()
