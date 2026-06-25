import csv
from collections import defaultdict

def analyze_data():
    transactions = defaultdict(list)
    
    with open('transactions-2000.csv', 'r') as f:
        reader = csv.reader(f)
        for row in reader:
            if len(row) < 4: continue
            trans_id = row[0]
            prod_id = row[3]
            transactions[trans_id].append(prod_id)
            
    # Calculate frequently bought together
    pairs = defaultdict(int)
    for trans_id, prods in transactions.items():
        prods = list(set(prods)) # unique products in transaction
        for i in range(len(prods)):
            for j in range(i+1, len(prods)):
                p1, p2 = prods[i], prods[j]
                if p1 > p2: p1, p2 = p2, p1
                pairs[f"{p1} & {p2}"] += 1
                
    sorted_pairs = sorted(pairs.items(), key=lambda x: x[1], reverse=True)
    print("Top 5 Frequently Bought Together Pairs:")
    for k, v in sorted_pairs[:5]:
        print(f"{k}: {v}")
        
    print(f"Total Transactions: {len(transactions)}")

if __name__ == '__main__':
    analyze_data()
