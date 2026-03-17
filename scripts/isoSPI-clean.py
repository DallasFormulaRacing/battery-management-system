import csv

def sanitize_isospi_structured(input_file, output_file):
    processed_rows = []
    
    try:
        with open(input_file, 'r') as f_in:
            reader = csv.DictReader(f_in)
            
            for row in reader:
                raw_data = row.get('decode_data', '').strip()
                
                # Verify this is a line with IC data
                if not raw_data or "IC1: [" not in raw_data:
                    continue
                
                try:
                    command = raw_data.split(',')[0].strip()
                    
                    meta_start = raw_data.find(',') + 1
                    meta_end = raw_data.find('IC1: [')
                    metadata = raw_data[meta_start:meta_end].strip().rstrip(',')
                    
                    ic1_raw = raw_data.split('IC1: [')[1].split(']')[0].strip()
                    ic2_raw = raw_data.split('IC2: [')[1].split(']')[0].strip()
                    
                    processed_rows.append([command, metadata, ic1_raw, ic2_raw])
                    
                except (IndexError, ValueError):
                    continue

        with open(output_file, 'w', newline='') as f_out:
            writer = csv.writer(f_out)
            writer.writerow(['Command', 'Metadata', 'IC1_Data', 'IC2_Data'])
            writer.writerows(processed_rows)
            
        print(f"Success! Cleaned data saved to: {output_file}")

    except FileNotFoundError:
        print(f"Error: Could not find {input_file}")

if __name__ == "__main__":
    INPUT_CSV = 'BMSLOG-3.14.csv'
    OUTPUT_CSV = 'sanitized_isospi.csv'
    
    sanitize_isospi_structured(INPUT_CSV, OUTPUT_CSV)