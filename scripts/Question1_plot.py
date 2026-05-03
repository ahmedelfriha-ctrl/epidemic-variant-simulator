from visualization import*

plot_individual_scenarios("data/R0_1_individuals.csv","Variation of SIR across time with R0=1")
plot_individual_scenarios("data/R0_high_individuals.csv","Variation of SIR across time with R0>1")
plot_individual_scenarios("data/R0_low_individuals.csv","Variation of SIR across time with R0<1")

plot_infected_comparison(["data/R0_low_individuals.csv","data/R0_high_individuals.csv","data/R0_1_individuals.csv"],"The variation of infected people across time (R0 variable)")


