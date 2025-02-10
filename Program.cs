using System;
using System.Collections.Generic;
using System.IO;
using System.Diagnostics; // For Stopwatch
using MathNet.Numerics.LinearAlgebra;

namespace VectorBinPacking_PfaffianDemo
{
    class Program
    {
        static string datasetPath = @"C:\Users\rlarb\C#_thesis_project\datasets\2-dimensional-benchmark";

        static void Main(string[] args)
        {
            string instanceFile = Path.Combine(datasetPath, "CL_01_25_10.txt");

            // Run the improved solver
            VBP_PfaffianSolver solver = new VBP_PfaffianSolver(instanceFile);
            solver.SolveWithMVVAndPfaffian(trials: 50);
        }
    }

    class VBP_PfaffianSolver
    {
        private List<Item> items;
        private double binCapacityWeight;
        private double binCapacityVolume;
        private List<Item> smallItems = new List<Item>();
        private List<Item> largeItems = new List<Item>();
        private List<Item> dummyItems = new List<Item>();  // New list for dummy items
        private int totalOperations = 0; // Total operations counter

        public VBP_PfaffianSolver(string filePath)
        {
            items = new List<Item>();
            LoadInstance(filePath);
            SeparateSmallAndLargeItems();
            AddDummyItemsIfNeeded();
        }

        private void LoadInstance(string filePath)
        {
            string[] lines = File.ReadAllLines(filePath);
            string[] binCapacities = lines[0].Split(' ');

            binCapacityWeight = double.Parse(binCapacities[0]);
            binCapacityVolume = double.Parse(binCapacities[1]);

            for (int i = 1; i < lines.Length; i++)
            {
                string[] parts = lines[i].Split(' ');
                int index = int.Parse(parts[0]);
                double weight = double.Parse(parts[2]);
                double volume = double.Parse(parts[3]);

                items.Add(new Item(index, weight, volume));
                totalOperations++; // Counting operation for adding an item
            }
        }

        private void SeparateSmallAndLargeItems()
        {
            foreach (var item in items)
            {
                if (item.Weight <= binCapacityWeight / 3.0 && item.Volume <= binCapacityVolume / 3.0)
                {
                    smallItems.Add(item);
                }
                else
                {
                    largeItems.Add(item);
                }
                totalOperations++; // Counting operation for checking each item
            }
        }

        private void AddDummyItemsIfNeeded()
        {
            int n = largeItems.Count;

            if (n % 2 == 1)
            {
                int dummyCount = 2 * ((n / 2) + 1) - n;  // Ensure even count
                for (int i = 0; i < dummyCount; i++)
                {
                    dummyItems.Add(new Item(-1, 0.0, 0.0)); // Dummy item with weight 0 and volume 0
                    totalOperations++; // Counting operation for adding dummy items
                }

                largeItems.AddRange(dummyItems);
                Console.WriteLine($"Added {dummyCount} dummy items to make largeItems count even.");
            }
        }

        private Matrix<double> BuildSkewSymmetricMatrix(bool useRandomWeights, Random rand)
        {
            int n = largeItems.Count;
            var A = Matrix<double>.Build.Dense(n, n, 0.0);
            int operationCount = 0;  // Local operation count for this function

            for (int i = 0; i < n; i++)
            {
                for (int j = i + 1; j < n; j++)
                {
                    double w = ComputeEdgeWeight(largeItems[i], largeItems[j], useRandomWeights, rand);
                    A[i, j] = w;
                    A[j, i] = -w;
                    operationCount++;  // Count the basic operation (assignment)
                    totalOperations++; // Increment the global operation count
                }
            }

            Console.WriteLine($"BuildSkewSymmetricMatrix executed {operationCount} basic operations.");
            return A;
        }

        private double ComputeEdgeWeight(Item a, Item b, bool useRandomWeights, Random rand)
        {
            if (a.Index == -1 || b.Index == -1)
            {
                return 0.0;
            }

            if (a.Weight + b.Weight > binCapacityWeight || a.Volume + b.Volume > binCapacityVolume)
            {
                return 0.0;
            }

            int smallCount = 0;
            foreach (var s in smallItems)
            {
                if (a.Weight + b.Weight + s.Weight <= binCapacityWeight && a.Volume + b.Volume + s.Volume <= binCapacityVolume)
                {
                    smallCount++;
                }
                totalOperations++; // Counting operations within the foreach loop
            }

            double baseWeight = 1.0 + smallCount;
            if (useRandomWeights)
            {
                double r = 0.1 + rand.NextDouble() * 9.9;
                return baseWeight * r;
            }
            else
            {
                return baseWeight;
            }
        }

        private bool IsPerfectMatchingExists(Matrix<double> A)
        {
            double detValue = A.Determinant();
            totalOperations++; // Counting the determinant calculation operation
            return Math.Abs(detValue) > 1e-12;
        }

        private int ApplyMVVAlgorithm(Matrix<double> A)
        {
            int n = largeItems.Count;
            int operationCount = 0; // Local operation count for this function

            double detValue = A.Determinant();
            totalOperations++; // Counting the determinant operation

            if (Math.Abs(detValue) > 1e-12)
            {
                operationCount++; // Count the operation for determining the perfect matching
                totalOperations++; // Counting the global operation count
                return n / 2;
            }

            return n;
        }

        public void SolveWithMVVAndPfaffian(int trials)
        {
            int n = largeItems.Count;
            if (n % 2 == 1)
            {
                Console.WriteLine("[Warning] Odd number of large items (n), perfect matching is not possible.");
                return;
            }

            Random rand = new Random();
            int bestBins = n;

            // Start measuring time
            Stopwatch stopwatch = new Stopwatch();
            stopwatch.Start();

            for (int t = 0; t < trials; t++)
            {
                Matrix<double> skewMatrix = BuildSkewSymmetricMatrix(useRandomWeights: true, rand);
                if (IsPerfectMatchingExists(skewMatrix))
                {
                    // Apply MVV algorithm to estimate minimum bins
                    int estimatedBins = ApplyMVVAlgorithm(skewMatrix);
                    bestBins = Math.Min(bestBins, estimatedBins); // Track the minimum bins
                }
            }

            // Stop measuring time
            stopwatch.Stop();
            Console.WriteLine($"Estimated minimum bins: {bestBins}");
            Console.WriteLine($"Execution Time: {stopwatch.ElapsedMilliseconds} ms");
            Console.WriteLine($"Total operations count: {totalOperations} operations.");
        }
    }

    class Item
    {
        public int Index { get; }
        public double Weight { get; }
        public double Volume { get; }

        public Item(int index, double weight, double volume)
        {
            Index = index;
            Weight = weight;
            Volume = volume;
        }
    }
}
