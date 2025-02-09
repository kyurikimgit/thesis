//using System;
//using System.Collections.Generic;
//using MathNet.Numerics.LinearAlgebra;

//namespace VectorBinPacking_PfaffianDemo
//{
//    class Program
//    {
//        static string datasetPath = @"C:\Users\rlarb\C#_thesis_project\datasets\2-dimensional-benchmark";

//        static void Main(string[] args)
//        {
//            string instanceFile = Path.Combine(datasetPath, "CL_01_25_10.txt");

//            // Run the improved solver
//            VBP_PfaffianSolver solver = new VBP_PfaffianSolver(instanceFile);
//            solver.SolveWithMVVAndPfaffian(trials: 50);
//        }
//    }

//    class VBP_PfaffianSolver
//    {
//        private List<Item> items;
//        private double binCapacityWeight;
//        private double binCapacityVolume;
//        private List<Item> smallItems = new List<Item>();
//        private List<Item> largeItems = new List<Item>();
//        private List<Item> dummyItems = new List<Item>();  // New list for dummy items

//        public VBP_PfaffianSolver(string filePath)
//        {
//            items = new List<Item>();
//            LoadInstance(filePath);
//            SeparateSmallAndLargeItems();
//            AddDummyItemsIfNeeded();
//        }

//        private void LoadInstance(string filePath)
//        {
//            string[] lines = File.ReadAllLines(filePath);
//            string[] binCapacities = lines[0].Split(' ');

//            binCapacityWeight = double.Parse(binCapacities[0]);
//            binCapacityVolume = double.Parse(binCapacities[1]);

//            for (int i = 1; i < lines.Length; i++)
//            {
//                string[] parts = lines[i].Split(' ');
//                int index = int.Parse(parts[0]);
//                double weight = double.Parse(parts[2]);
//                double volume = double.Parse(parts[3]);

//                items.Add(new Item(index, weight, volume));
//            }
//        }

//        private void SeparateSmallAndLargeItems()
//        {
//            foreach (var item in items)
//            {
//                if (item.Weight <= binCapacityWeight / 3.0 && item.Volume <= binCapacityVolume / 3.0)
//                {
//                    smallItems.Add(item);
//                }
//                else
//                {
//                    largeItems.Add(item);
//                }
//            }
//        }

//        // Step 1: Add dummy items if largeItems count is odd
//        private void AddDummyItemsIfNeeded()
//        {
//            int n = largeItems.Count;

//            // If largeItems count is odd, add dummy items
//            if (n % 2 == 1)
//            {
//                int dummyCount = 2 * ((n / 2) + 1) - n;  // Ensure even count
//                for (int i = 0; i < dummyCount; i++)
//                {
//                    dummyItems.Add(new Item(-1, 0.0, 0.0)); // Dummy item with weight 0 and volume 0
//                }

//                // Add dummy items to largeItems list
//                largeItems.AddRange(dummyItems);
//                Console.WriteLine($"Added {dummyCount} dummy items to make largeItems count even.");
//            }
//        }

//        private Matrix<double> BuildSkewSymmetricMatrix(bool useRandomWeights, Random rand)
//        {
//            int n = largeItems.Count;
//            var A = Matrix<double>.Build.Dense(n, n, 0.0);

//            for (int i = 0; i < n; i++)
//            {
//                for (int j = i + 1; j < n; j++)
//                {
//                    double w = ComputeEdgeWeight(largeItems[i], largeItems[j], useRandomWeights, rand);
//                    A[i, j] = w;
//                    A[j, i] = -w;
//                }
//            }

//            return A;
//        }

//        private double ComputeEdgeWeight(Item a, Item b, bool useRandomWeights, Random rand)
//        {
//            // If any of the items are dummy, they shouldn't participate in matching.
//            if (a.Index == -1 || b.Index == -1) // Dummy items are marked by -1 index
//            {
//                return 0.0;
//            }

//            if (a.Weight + b.Weight > binCapacityWeight || a.Volume + b.Volume > binCapacityVolume)
//            {
//                return 0.0;
//            }

//            int smallCount = 0;
//            foreach (var s in smallItems)
//            {
//                if (a.Weight + b.Weight + s.Weight <= binCapacityWeight && a.Volume + b.Volume + s.Volume <= binCapacityVolume)
//                {
//                    smallCount++;
//                }
//            }

//            double baseWeight = 1.0 + smallCount;
//            if (useRandomWeights)
//            {
//                double r = 0.1 + rand.NextDouble() * 9.9;
//                return baseWeight * r;
//            }
//            else
//            {
//                return baseWeight;
//            }
//        }

//        private bool IsPerfectMatchingExists(Matrix<double> A)
//        {
//            double detValue = A.Determinant();
//            return Math.Abs(detValue) > 1e-12;
//        }

//        // Step 4: Apply MVV Algorithm - Find maximum matching
//        private int ApplyMVVAlgorithm(Matrix<double> A)
//        {
//            int n = largeItems.Count;

//            // Step 1: Tutte Matrix Construction
//            // For now, we're directly working with the adjacency matrix A.
//            // Ideally, we should construct a Tutte matrix here with indeterminates.

//            // Step 2: Apply random edge weights (already handled in ComputeEdgeWeight)

//            // Step 3: Compute Pfaffian or Determinant
//            double detValue = A.Determinant();

//            if (Math.Abs(detValue) > 1e-12) // If determinant is non-zero, a perfect matching exists
//            {
//                // Compute the perfect matching
//                return n / 2;  // A placeholder for now
//            }

//            // No perfect matching found
//            return n;  // In case of failure, return the worst case (n bins)
//        }

//        // Step 7: Monte Carlo method for multiple trials and checking Pfaffian(=det)≠0
//        public void SolveWithMVVAndPfaffian(int trials)
//        {
//            int n = largeItems.Count;
//            if (n % 2 == 1)
//            {
//                Console.WriteLine("[Warning] Odd number of large items (n), perfect matching is not possible.");
//                return;
//            }

//            Random rand = new Random();
//            int bestBins = n;

//            for (int t = 0; t < trials; t++)
//            {
//                Matrix<double> skewMatrix = BuildSkewSymmetricMatrix(useRandomWeights: true, rand);
//                if (IsPerfectMatchingExists(skewMatrix))
//                {
//                    // Apply MVV algorithm to estimate minimum bins
//                    int estimatedBins = ApplyMVVAlgorithm(skewMatrix);
//                    bestBins = Math.Min(bestBins, estimatedBins);
//                }
//            }

//            Console.WriteLine($"Estimated minimum bins: {bestBins}");
//        }
//    }

//    class Item
//    {
//        public int Index { get; }
//        public double Weight { get; }
//        public double Volume { get; }

//        public Item(int index, double weight, double volume)
//        {
//            Index = index;
//            Weight = weight;
//            Volume = volume;
//        }
//    }
//}

// (위 코드에