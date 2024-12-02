import pandas as pd 
import numpy as np
import matplotlib.pyplot as plt
import csv
x=[]
y=[]

with open('2cars7seats.csv') as csvfile:
    plots = csv.reader(csvfile, delimiter= ',')
    
    for row in plots:
        x.append([0])
        y.append(row[1])
        
plt.plot(x,y, marker = 'o')
plt.xlabel('Minutes') 
plt.ylabel('Total People') 
plt.title('Total Arrival') 
plt.legend() 
plt.show() 