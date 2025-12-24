import math

def get_theoretical_response_time():
    # --- M/M/k SYSTEM PARAMETERS (These are the same as in the Theoretical-Test configuration on OMNeT++) ---
    N = 64.0        # total Users
    T = 10.0        # Interarrival time (s) -> E[T]
    D = 5.0         # Service duration (s) -> E[D]
    C = 40.0        # total Capacity (GHz)
    R_job = 1.0     # single job capacity occupancy (GHz)

    # --- QUEUEING PARAMETERS DERIVATION ---
    # Arrival Rate (Lambda)
    lam = N / T   # 6.4 job/s

    # Service Rate (Mu) for a single server
    mu = 1.0 / D  # 0.2 job/s

    # number of Servers (k)
    k = int(C / R_job)

    # Utilization (Rho)
    # WARNING: Rho must be < 1 in order to have stability
    rho = lam / (k * mu)

    # Traffic (u)
    u = lam/mu

    print(f"--- DERIVED PARAMETERS ---")
    print(f"Lambda (Arrivals): {lam} job/s")
    print(f"Mu (Service):   {mu} job/s")
    print(f"k (Servers):    {k}")
    print(f"Rho (Load):    {rho:.4f} ({(rho*100):.1f}%)")
    print(f"u (Traffic):    {u} Erlang")

    if rho >= 1.0:
        return "THE SYSTEM IS UNSTABLE! (Rho >= 1)"

    # --- ERLANG-C FORMULA ---
    # P0 = Probability of having 0 Users in the system
    # We need a sum for the first k-1 factors
    sum_part = 0
    for n in range(k):
        sum_part += ((k * rho)**n) / math.factorial(n)

    # n=k factor (Erlang-C specific equation)
    # (k * rho) = u -> this is simply because:
    # rho = lambda/(k * mu)
    # u = lambda/mu
    # so: rho * k = (lambda/(k * mu)) * k = lambda/mu = u
    last_term = ((u)**k) / (math.factorial(k) * (1 - rho))

    # final P0
    p0 = 1.0 / (sum_part + last_term)

    # --- PERFORMANCE METRICS COMPUTATION ---

    # Probability for a workload to wait in queue (Erlang-C probability)
    # For a reference to this equation see https://en.wikipedia.org/wiki/Erlang_(unit)
    p_queue = last_term * p0

    # Mean number of workloads inside of the queue (E[Nq] o Lq)
    # Equation: Lq = P(Queue) * rho / (1 - rho)
    en_q = (p_queue * rho) / (1 - rho)

    # Mean Waiting Time (E[W]) - this comes from Little's Law
    # E[W] = Lq / Lambda
    e_w = en_q / lam

    # Mean Response Time (E[R])
    # E[R] = E[W] + E[S]  (dove E[S] = D = 1/mu)
    e_r = e_w + (1.0 / mu)

    return e_r

# computation
theoretical_value = get_theoretical_response_time()

print(f"\n=== FINAL RESULT ===")
print(f"Theoretical Mean Response Time (E[R]): {theoretical_value:.5f} seconds")
