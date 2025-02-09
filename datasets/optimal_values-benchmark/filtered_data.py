# import pandas as pd

# # 1️ Load the CSV file (using semicolon `;` as a delimiter)
# file_path = r"C:\Users\rlarb\C#_thesis_project\datasets\optimal_values-benchmark\optimal_values_larger_demand.csv"
# df = pd.read_csv(file_path, delimiter=";")  # Add `delimiter=";"` to correctly parse the file

# # 2️ Apply filtering conditions
# df = df[df["dimension"] != 20]  # Remove rows where 'dimension' is 20
# df = df[df["items"].isin([24, 25, 200, 201])]  # Keep only rows where 'items' is in [24, 25, 200, 201]

# # 3️ Save the filtered data
# filtered_file_path = r"C:\Users\rlarb\C#_thesis_project\datasets\optimal_values-benchmark\optimal_values_filtered.csv"
# df.to_csv(filtered_file_path, index=False, sep=";")  # Use `;` as a separator when saving

# print(f"Filtering complete. File saved at: {filtered_file_path}")

import pandas as pd

# Load the CSV file (using semicolon `;` as a delimiter)
file_path = r"C:\Users\rlarb\C#_thesis_project\datasets\optimal_values-benchmark\optimal_values_larger_demand.csv"
df = pd.read_csv(file_path, delimiter=";")  # Use `;` as a delimiter to read the original file

# Apply filtering conditions
df = df[df["dimension"] != 20]  # Remove rows where 'dimension' is 20
df = df[df["items"].isin([24, 25, 200, 201])]  # Keep only rows where 'items' is in [24, 25, 200, 201]

# Convert 'instance_number' to an integer (remove decimals)
df["instance_number"] = df["instance_number"].astype(int)

# Save the filtered data with space as the separator
filtered_file_path = r"C:\Users\rlarb\C#_thesis_project\datasets\optimal_values-benchmark\optimal_values_filtered.txt"
df.to_csv(filtered_file_path, index=False, sep=" ")  # Use space as a separator

# Save column descriptions to a readme file
readme_path = r"C:\Users\rlarb\C#_thesis_project\datasets\optimal_values-benchmark\readme.txt"
column_descriptions = """dimension: Number of dimensions
class: Item classification
items: Number of items
instance_number: Unique instance identifier (integer)
LB: Lower bound
UB: Upper bound
BKLB: Best known lower bound
BKUB: Best known upper bound
"""

with open(readme_path, "w") as f:
    f.write(column_descriptions)

print(f"Filtering complete. File saved at: {filtered_file_path}")
print(f"Column descriptions saved at: {readme_path}")
