## Code Flow Summary

### 1. Data Loading & Preprocessing

#### Loading Dataset (LoadInstance)
- Reads input files (e.g., CL_01_25_10.txt) to extract bin capacities (weight & volume) and item properties.
- Tracks the number of computational operations using `totalOperations`.

#### Classifying Items (SeparateSmallAndLargeItems)
- Items are categorized into `smallItems` (weight/volume ≤ 1/3 of bin capacity) and `largeItems` (the rest).

#### Adding Dummy Items if Necessary (AddDummyItemsIfNeeded)
- If the number of `largeItems` is odd, dummy items (weight=0, volume=0) are added to ensure an even count for pairing.

### 2. Constructing the Skew-Symmetric Matrix (Pfaffian) & Checking Perfect Matching

#### Building Skew-Symmetric Matrix (BuildSkewSymmetricMatrix)
- Constructs a skew-symmetric adjacency matrix (`Aij = -Aji`) for the `largeItems` list.
- Computes edge weights using `ComputeEdgeWeight`.
- Tracks operations with `operationCount` (local) and `totalOperations` (global).

#### Computing Edge Weights (ComputeEdgeWeight)
- Determines whether two items can fit into the same bin.
- If they can, checks how many small items can be packed with them.
- If `useRandomWeights=True`, applies a random factor to edge weights to support Monte Carlo trials.

#### Checking for Perfect Matching (Pfaffian Calculation) (IsPerfectMatchingExists)
- Computes the determinant of matrix `A`.
- If `det(A) ≠ 0`, a perfect matching exists, meaning all items can be paired optimally.

### 3. Estimating the Minimum Number of Bins with the MVV Algorithm

#### Applying the MVV Algorithm (ApplyMVVAlgorithm)
- Uses Pfaffian results to estimate the minimum number of bins required.
- If `det(A) ≠ 0`, assumes items can be perfectly matched, requiring `n/2` bins.

### 4. Monte Carlo Simulations & Final Result Calculation

#### Running Monte Carlo Trials (SolveWithMVVAndPfaffian)
- Repeats the matching process 50 times to find the best bin-packing estimate.
- Uses `Stopwatch` to measure execution time and tracks computational complexity via `totalOperations`.
- Outputs the minimum estimated bin count (`bestBins`).

### Example Output

```
Added 2 dummy items to make largeItems count even.

BuildSkewSymmetricMatrix executed 300 basic operations.

Estimated minimum bins: 25

Execution Time: 1523 ms

Total operations count: 5232 operations.
```