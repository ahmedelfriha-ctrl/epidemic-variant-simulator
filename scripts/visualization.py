"""
=============================================================================
visualization.py
=============================================================================
Companion script for the SIR Epidemic Simulation.
Provides utilities to generate starting network topologies (via NetworkX) 
and visualize the CSV output data from the C simulation engine.
=============================================================================
"""

import os
import pandas as pd
import matplotlib.pyplot as plt
import networkx as nx

def export_graph(n, beta, gamma, filename, network=""):
    """Generates a network topology and exports it to the simulation's input format."""
    
    if network == "small_world":
        G = nx.watts_strogatz_graph(n, 6, 0.1)
    elif network == "scale_free":
        G = nx.barabasi_albert_graph(n, 5)
    elif network == "random_geometry":
        G = nx.random_geometric_graph(n, 0.055)
    elif network == "connected_caveman":
        num_cliques = max(1, n // 5)
        G = nx.connected_caveman_graph(num_cliques, 5)
    else:
        # Default fallback
        G = nx.erdos_renyi_graph(n, 0.5)

    with open(filename, 'w') as f:
        f.write(f"{n} {beta} {gamma}\n")
        
        # Node 0 starts as Infected, all others are Susceptible
        f.write("0 I\n")
        for i in range(1, n):
            f.write(f"{i} S\n")
            
        # Write all undirected edges
        for u, v in G.edges():
            f.write(f"{u} {v}\n")

def plot_individual_scenarios(filename, title):
    """Plots the Susceptible, Infected, and Recovered curves for a single scenario."""
    plt.figure(figsize=(10, 6))
    df = pd.read_csv(filename)
    
    plt.plot(df['Time'], df['Susceptible'], color='green', label='Susceptible', linewidth=2)
    plt.plot(df['Time'], df['Infected'], color='red', label='Infected', linewidth=2)
    plt.plot(df['Time'], df['Recovered'], color='blue', label='Recovered', linewidth=2)
    
    # Highlight the peak of the infection
    peak_val = df['Infected'].max()
    peak_time = df.loc[df['Infected'].idxmax(), 'Time']
    max_pop = df[['Susceptible', 'Infected', 'Recovered']].max().max()
    
    plt.axvline(x=peak_time, color='gray', linestyle='--', alpha=0.5)
    plt.annotate(
        f'Peak: {int(peak_val)}\nTime: {int(peak_time)}', 
        xy=(peak_time, peak_val), 
        xytext=(peak_time + (df['Time'].max() * 0.05), peak_val + (max_pop * 0.05)),
        arrowprops=dict(facecolor='black', shrink=0.05, width=1, headwidth=5),
        bbox=dict(boxstyle="round,pad=0.3", fc="yellow", alpha=0.3)
    )
    
    plt.title(title, fontsize=14, fontweight='bold')
    plt.xlabel("Time Steps", fontsize=12)
    plt.ylabel("Population", fontsize=12)
    plt.legend(frameon=True, shadow=True)
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    
    plt.savefig(f"{title}.png", dpi=300)   
    plt.close()       

def plot_infected_comparison(filenames, title):
    """Overlays the Infected curves from multiple scenarios for direct comparison."""
    plt.figure(figsize=(10, 6))

    for filename in filenames:
        df = pd.read_csv(filename)
        

        base = os.path.basename(filename)
        label_name = base.replace("_individuals", "").replace(".csv", "")

        plt.plot(df['Time'], df['Infected'], linewidth=2.5, label=label_name)

        # Highlight the peak for each curve
        peak_val = df['Infected'].max()
        peak_time = df.loc[df['Infected'].idxmax(), 'Time']
    
        plt.plot(peak_time, peak_val, marker='o', markersize=5, color='black', alpha=0.5)

    plt.title(title, fontsize=14, fontweight='bold')
    plt.xlabel("Time Steps", fontsize=12)
    plt.ylabel("Number of Infected Individuals", fontsize=12)
    
    plt.legend(frameon=True, shadow=True)
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    
    plt.savefig(f"{title}.png", dpi=300)
    plt.close()

def plot_dominant_variants(variants_file="data/variants.csv", title="Dominant Variants", top_n=None):
    """Plots a bar chart of the most successful viral variants and their R0 values."""
    df = pd.read_csv(variants_file)
    
    # Calculate R0 = beta/gamma for each variant
    df['R0'] = df['beta'] / df['gamma']
    
    # Sort by total_infected in descending order
    df_sorted = df.sort_values('total_infected', ascending=False)
    
    if top_n is not None:
        df_sorted = df_sorted.head(top_n)
    
    labels = [f"ID: {int(row['variant'])}\nR0: {row['R0']:.2f}" for _, row in df_sorted.iterrows()]
    
    fig, ax = plt.subplots(figsize=(14, 6))
    bars = ax.bar(range(len(df_sorted)), df_sorted['total_infected'], 
                   color='steelblue', edgecolor='navy', alpha=0.7)
    
    # Add value labels on top of bars
    for bar, val in zip(bars, df_sorted['total_infected']):
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height,
                f'{int(val)}',
                ha='center', va='bottom', fontsize=9, fontweight='bold')
    
    ax.set_xticks(range(len(df_sorted)))
    ax.set_xticklabels(labels, fontsize=9)
    
    ax.set_xlabel("Variant (ID and R0)", fontsize=12, fontweight='bold')
    ax.set_ylabel("Total Infected Individuals", fontsize=12, fontweight='bold')
    ax.set_title(title, fontsize=14, fontweight='bold')
    ax.grid(True, axis='y', linestyle='--', alpha=0.6)
    
    plt.tight_layout()
    plt.savefig(f"{title}.png", dpi=300)
    plt.close()