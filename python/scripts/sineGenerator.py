import math

frequency = 50 
sampling_frequency = 1000  
num_samples = 120
amplitude = 500  
dc_offset = 511
output_filename = "sine_wave_samples.txt"
sine_table = []  # Lookup table

def generate_sine_table():
    for i in range(num_samples):
        time = i / sampling_frequency  
        angle = (2 * math.pi * frequency * time)  
        sample = round(dc_offset + amplitude * math.sin(angle))  
        sine_table.append(sample)

def write_samples_to_file():
    with open(output_filename, "w") as file:
        file.write("unsigned short sineSamples[SINE_PERIOD_STEPS] = {")
        for i in range(num_samples):
            sample = sine_table[i]  
            print(sample)
            binary = bin(sample & 0b1111111111)[2:].zfill(10)  # Convert the sample to binary format
            binary = '0000' + binary + '00'  # Append four 0 bits to the left and two to the right
            
            if i < num_samples - 1:
              file.write(f"0b{binary}, ")
            else:
              file.write(f"0b{binary}")
        file.write("};\n")

def main():
    generate_sine_table()  
    write_samples_to_file() 

if __name__ == "__main__":
    main()

