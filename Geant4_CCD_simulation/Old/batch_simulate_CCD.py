import csv
import subprocess
from multiprocessing import Pool, cpu_count
import os
import sys
# Define the header of the config.mac file
config_header = """\
# This file is auto-generated
# It configures the particle type, energy, number of particles
# and optionally the angles of the particles
"""

element_symbols = ["h", "he", "li", "be", "b", "c", "n", "o", "f", "ne", "na", "mg", "al", "si", "p", "s", "cl", "ar", "k", "ca", "sc", "ti", "v", "cr", "mn", "fe", "co", "ni", "cu", "zn", "ga", "ge", "as", "se", "br", "kr", "rb", "sr", "y", "zr", "nb", "mo", "tc", "ru", "rh", "pd", "ag", "cd", "in", "sn", "sb", "te", "i", "xe", "cs", "ba", "la", "ce", "pr", "nd", "pm", "sm", "eu", "gd", "tb", "dy", "ho", "er", "tm", "yb", "lu", "hf", "ta", "w", "re", "os", "ir", "pt", "au", "hg", "tl", "pb", "bi", "po", "at", "rn", "fr", "ra", "ac", "th", "pa", "u", "np", "pu", "am", "cm", "bk", "cf", "es", "fm", "md", "no", "lr", "rf", "db", "sg", "bh", "hs", "mt", "ds ", "rg ", "cn ", "nh", "fl", "mc", "lv", "ts", "og"]

simulation_dir = 'build' 

def symbol_to_ZA(isotope_str):
    element_sym = ''.join(filter(str.isalpha, isotope_str)).lower()
    numbers_in_string = ''.join(filter(str.isdigit, isotope_str))
    if len(numbers_in_string) == 0:
        return None, None
    element_A = int(numbers_in_string)
    for idx, symbol in enumerate(element_symbols):
        if symbol == element_sym:
            Z = idx + 1  # Z is one more than the index position
            A = element_A
            return Z, A
    return None, None  # return None if no match found

def run_simulation(row):
    # Define the parameters based on the CSV row
    particle = row['Particle']
    energy = row['Energy']
    particles = row['Particles']
    set_angle = row.get('SetAngle', '0')
    phi = row.get('Phi', '0.0')
    theta = row.get('Theta', '0.0')

    use_ion_spectrum = 'false'
    particle_source = 'gun'
    particle_source_line = f'/{particle_source}/particle {particle}'
    energy_line = f'/gun/energy {energy} keV'

    Z, A = symbol_to_ZA(particle)
    if Z is not None and A is not None:
        use_ion_spectrum = 'true'
        # particle_source = 'gps'
        particle_source_line = ""
        energy_line = f'/myapp/setIonZA {Z} {A}'
    
    angle_line = ""
    if set_angle == '1':
        angle_line = f'/myapp/setParticleAngles {phi} {theta} {1}'
    particle_line = ""

    config_content = f"""\
{config_header}
{particle_source_line}
/myapp/useRadioactiveIon {use_ion_spectrum}
{energy_line}
{angle_line}
/run/beamOn {particles}
"""

    # Write the config.mac file
    if set_angle == '1':
        config_filename = f'config_{row["Particles"]}_{particle}_{energy}_phi{phi}_theta{theta}.mac'
    else:
        config_filename = f'config_{row["Particles"]}_{particle}_{energy}.mac'
    with open(os.path.join(simulation_dir, config_filename), 'w') as config_file:
        config_file.write(config_content)

    # Run the simulation with this configuration
    subprocess.run(['./Geant4Simulation', config_filename], check=True, cwd=simulation_dir)


if __name__ == "__main__":
    # Check if the script has the correct number of arguments
    if len(sys.argv) != 2:
        print("Usage: python3 batch_simulate_CCD.py <csv_file>")
        sys.exit(1)

    # The script expects the filename as the first command-line argument
    csv_filename = sys.argv[1]

    # Read the CSV file
    with open(csv_filename, 'r') as csv_file:
        csv_reader = list(csv.DictReader(csv_file, delimiter=','))

        # Create a multiprocessing Pool and run the simulations in parallel
        with Pool(cpu_count()) as p:
            p.map(run_simulation, csv_reader)
