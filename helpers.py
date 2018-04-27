import numpy as np

EPS = 1e-5
BOUNDARY = 1 - EPS

def pullback(poincare_pts):
    norms = np.sqrt((poincare_pts ** 2).sum(axis=1))
    new_norms = norms.copy()
    print('%i vectors are on the boundary (they will be pulled back)' % (norms > BOUNDARY).sum())
    new_norms[norms > BOUNDARY] = BOUNDARY
    multipliers = new_norms / norms
    return poincare_pts * multipliers[:,np.newaxis]

def to_hyperboloid_points(poincare_pts):
    """
    Post: result.shape[1] == poincare_pts.shape[1] + 1
    """
    norm_sqd = (poincare_pts ** 2).sum(axis=1)
    N = poincare_pts.shape[1]
    result = np.zeros((poincare_pts.shape[0], N + 1), dtype=np.float64)
    result[:,:N] = (2. / (1 - norm_sqd))[:,np.newaxis] * poincare_pts
    result[:,N] = (1 + norm_sqd) / (1 - norm_sqd)
    return result

def minkowski_dot_matrix(vecs_a, vecs_b):
    """
    Return the matrix giving the Minkowski dot product of every vector in
    vecs_a with every vector in vecs_b.
    """
    rank = vecs_a.shape[1] - 1
    euc_dps = vecs_a[:,:rank].dot(vecs_b[:,:rank].T)
    timelike = vecs_a[:,rank][:,np.newaxis].dot(vecs_b[:,rank][:,np.newaxis].T)
    return euc_dps - timelike

def hyperbolic_distance(hyperboloid_vec, hyperboloid_vecs):
    mdps = minkowski_dot_matrix(hyperboloid_vec[np.newaxis,:], hyperboloid_vecs)[0]
    return np.arccosh(-mdps + 1e-5)  # add cst for stability (arccosh is monotone)
