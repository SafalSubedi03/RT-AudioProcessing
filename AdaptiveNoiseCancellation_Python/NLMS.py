import numpy as np

# ---------------- NLMS functions ----------------

def compute_y(wk, u, n):
    """
    Computes y[n] = sum_k wk[k] * u[n-k]
    Arguments:
        wk : NumPy array of filter weights (length M)
        u  : NumPy array of reference signal (1D)
        n  : current sample index
    Returns:
        y[n] : predicted far-end signal at sample n
    """
    M = len(wk)
    y_n = 0.0
    for k in range(M):
        if n - k >= 0:
            y_n += wk[k] * u[n - k]
    return y_n


def updatewk(wk, u, n, mu, e):
    """
    Updates NLMS weights using error signal e[n]
    Arguments:
        wk : NumPy array of filter weights (length M)
        u  : NumPy array of reference signal (1D)
        n  : current sample index
        mu : step size
        e  : error signal at sample n
    Returns:
        wk : updated filter weights (NumPy array)
    """
    M = len(wk)
    # Compute normalization factor (power of input vector)
    Power = 1e-12  # small value to avoid division by zero
    for k in range(M):
        if n - k >= 0:
            Power += u[n - k] ** 2

    # Update each weight
    for k in range(M):
        if n - k >= 0:
            wk[k] += (mu / Power) * e * u[n - k]

    return wk
