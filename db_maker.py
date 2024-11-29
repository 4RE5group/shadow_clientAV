import sqlite3
import csv

def remove_null_bytes(input_path, output_path):
    with open(input_path, "rb") as infile, open(output_path, "wb") as outfile:
        for line in infile:
            outfile.write(line.replace(b'\0', b''))

def export_sorted_malware_list_csv(db_path, output_file):
    # Write to the output file
    i = 0
    with open(output_file, "w", encoding="utf-8") as file:
        with open(db_path, encoding="latin1") as csvfile:  # Use latin1 to avoid decode errors
            reader = csv.DictReader(csvfile, delimiter=',', quotechar='"')
            for row in reader:
                if row and row != None and row != "" and "md5_hash" in row.keys() and row["md5_hash"] != None:
                    hash_value = row["md5_hash"].replace('"', '')
                    name = "MalwareBazaar[found by " + row["reporter"].replace('"', '').strip() +"]-" + row["file_name"].replace('"', '').strip() + "-" + row["file_type_guess"].replace('"', '').strip()
                    if hash_value[0] == ' ':
                        hash_value = hash_value[1:]
                    name = name.replace(":", "-")
                    file.write(f"{hash_value}: {name}\n")
                    i += 1

    print(f"Export completed. Rows written to {output_file} total {i}")
    


def export_sorted_malware_list(db_path, output_file): # sql
    try:
        # Connect to the SQLite database
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Query the malwareList table
        cursor.execute("SELECT hash, name FROM malwareList ORDER BY hash ASC")
        rows = cursor.fetchall()

        # Write to the output file
        i=0
        with open(output_file, "w") as file:
            for row in rows:
                # if i>=20:
                    # break
                hash_value, name = row
                if hash_value[0] == ' ':
                    hash_value = hash_value[1:]
                name = name.replace(":", "-")
                file.write(f"{hash_value}: {name}\n")
                i+=1

        print(f"Export completed. Rows written to {output_file} total {i}")
    
    except sqlite3.Error as e:
        print(f"Database error: {e}")
    
    finally:
        # Ensure the connection is closed
        if conn:
            conn.close()

if __name__ == "__main__":
    # Define the database path and output file
    db_path = "full.csv"  # Replace with your database path
    output_file = "malwareBazaar.txt"

    export_sorted_malware_list_csv(db_path, output_file)
    # remove_null_bytes(d)
