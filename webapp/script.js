let products = [];
let vectorTransactions = []; // Array to simulate std::vector
let mapTransactionsArray = []; // Base array for Hash Map
let mapIndex = new Map(); // Simulating unordered_map<string, array_of_indexes>

// Utils
function parseCSV(text) {
    const lines = text.trim().split('\n');
    const headers = lines[0].split(',').map(h => h.trim());
    return lines.slice(1).map(line => {
        const values = line.split(',');
        let obj = {};
        headers.forEach((h, i) => obj[h] = values[i] ? values[i].trim() : '');
        return obj;
    });
}

// 1. Data Loading
document.getElementById('btn-load').addEventListener('click', () => {
    const fileProd = document.getElementById('file-products').files[0];
    const fileTrans = document.getElementById('file-transactions').files[0];
    const status = document.getElementById('load-status');

    if (!fileProd || !fileTrans) {
        status.textContent = "Error: Harap upload kedua file CSV (Products & Transactions) terlebih dahulu.";
        return;
    }

    status.textContent = "Memproses file...";

    const reader1 = new FileReader();
    reader1.onload = (e) => {
        products = parseCSV(e.target.result);
        
        const reader2 = new FileReader();
        reader2.onload = (ev) => {
            const txData = parseCSV(ev.target.result);
            
            // Build Vector
            const startVec = performance.now();
            vectorTransactions = [];
            for (let i = 0; i < txData.length; i++) {
                vectorTransactions.push(txData[i]);
            }
            const timeVec = performance.now() - startVec;

            // Build Map
            const startMap = performance.now();
            mapTransactionsArray = [];
            mapIndex = new Map();
            for (let i = 0; i < txData.length; i++) {
                mapTransactionsArray.push(txData[i]);
                const txId = txData[i].transactionId;
                if (!mapIndex.has(txId)) mapIndex.set(txId, []);
                mapIndex.get(txId).push(i);
            }
            const timeMap = performance.now() - startMap;

            status.innerHTML = `✅ Berhasil memuat <strong>${products.length}</strong> Produk & <strong>${txData.length}</strong> Transaksi.<br>
                Waktu Load Vector: ${timeVec.toFixed(2)} ms | Waktu Load Map: ${timeMap.toFixed(2)} ms`;
            
            // Enable ops
            document.getElementById('ops-section').style.opacity = "1";
            document.getElementById('ops-section').style.pointerEvents = "auto";
        };
        reader2.readAsText(fileTrans);
    };
    reader1.readAsText(fileProd);
});

// Tab UI
function switchTab(tabId) {
    document.querySelectorAll('.tab-content').forEach(el => el.classList.remove('active'));
    document.querySelectorAll('.tab-btn').forEach(el => el.classList.remove('active'));
    document.getElementById('tab-' + tabId).classList.add('active');
    event.currentTarget.classList.add('active');
}

// 2. Manual Operations Simulation
function estimateMemory(vecLen, mapLen, mapKeys) {
    // 1 struct ~ 100 bytes
    const vecBytes = vecLen * 100;
    // Map = array bytes + map overhead (~64 bytes per key + 8 bytes per array element pointer)
    const mapBytes = (mapLen * 100) + (mapKeys * 64) + (mapLen * 8);
    return { vecBytes, mapBytes };
}

// To get measurable time for sub-millisecond JS ops, we run search multiple times
const REPEAT_COUNT = 1000; 

function doOperation(opType) {
    if (vectorTransactions.length === 0) return alert("Silakan Muat Data terlebih dahulu.");

    let timeVec = 0;
    let timeMap = 0;
    
    if (opType === 'search') {
        const txId = document.getElementById('search-tx').value;
        if (!txId) return alert("Isi Transaction ID");

        // Vector Search
        const vStart = performance.now();
        for(let k=0; k<REPEAT_COUNT; k++) {
            let res = [];
            for (let i = 0; i < vectorTransactions.length; i++) {
                if (vectorTransactions[i].transactionId === txId) res.push(vectorTransactions[i]);
            }
        }
        timeVec = performance.now() - vStart;

        // Map Search
        const mStart = performance.now();
        for(let k=0; k<REPEAT_COUNT; k++) {
            let res = [];
            const indexes = mapIndex.get(txId);
            if (indexes) {
                for (let i = 0; i < indexes.length; i++) res.push(mapTransactionsArray[indexes[i]]);
            }
        }
        timeMap = performance.now() - mStart;

    } else if (opType === 'insert') {
        const item = {
            transactionId: document.getElementById('ins-tx').value,
            customerId: document.getElementById('ins-cust').value,
            productId: document.getElementById('ins-prod').value,
            quantity: document.getElementById('ins-qty').value
        };

        // Vector Insert
        const vStart = performance.now();
        vectorTransactions.push(item);
        timeVec = performance.now() - vStart;

        // Map Insert
        const mStart = performance.now();
        const idx = mapTransactionsArray.length;
        mapTransactionsArray.push(item);
        if (!mapIndex.has(item.transactionId)) mapIndex.set(item.transactionId, []);
        mapIndex.get(item.transactionId).push(idx);
        timeMap = performance.now() - mStart;

    } else if (opType === 'update') {
        const txId = document.getElementById('upd-tx').value;
        const prodId = document.getElementById('upd-prod').value;
        const newQty = document.getElementById('upd-qty').value;

        // Vector Update
        const vStart = performance.now();
        for(let k=0; k<REPEAT_COUNT; k++) {
            for (let i = 0; i < vectorTransactions.length; i++) {
                if (vectorTransactions[i].transactionId === txId && vectorTransactions[i].productId === prodId) {
                    vectorTransactions[i].quantity = newQty;
                    break;
                }
            }
        }
        timeVec = performance.now() - vStart;

        // Map Update
        const mStart = performance.now();
        for(let k=0; k<REPEAT_COUNT; k++) {
            const indexes = mapIndex.get(txId);
            if (indexes) {
                for (let i = 0; i < indexes.length; i++) {
                    if (mapTransactionsArray[indexes[i]].productId === prodId) {
                        mapTransactionsArray[indexes[i]].quantity = newQty;
                        break;
                    }
                }
            }
        }
        timeMap = performance.now() - mStart;

    } else if (opType === 'delete') {
        const txId = document.getElementById('del-tx').value;
        const prodId = document.getElementById('del-prod').value;

        // Note: For Delete, doing it 1000 times will fail after 1st. 
        // We do it once.
        const vStart = performance.now();
        for (let i = 0; i < vectorTransactions.length; i++) {
            if (vectorTransactions[i].transactionId === txId && vectorTransactions[i].productId === prodId) {
                vectorTransactions.splice(i, 1);
                break;
            }
        }
        timeVec = performance.now() - vStart;

        const mStart = performance.now();
        const indexes = mapIndex.get(txId);
        if (indexes) {
            for (let i = 0; i < indexes.length; i++) {
                let actualIdx = indexes[i];
                if (mapTransactionsArray[actualIdx].productId === prodId) {
                    mapTransactionsArray.splice(actualIdx, 1);
                    // Rebuild Map index to mimic C++ O(N) deletion penalty
                    mapIndex.clear();
                    for(let j=0; j<mapTransactionsArray.length; j++) {
                        let tid = mapTransactionsArray[j].transactionId;
                        if(!mapIndex.has(tid)) mapIndex.set(tid, []);
                        mapIndex.get(tid).push(j);
                    }
                    break;
                }
            }
        }
        timeMap = performance.now() - mStart;
    }

    // Display
    const mem = estimateMemory(vectorTransactions.length, mapTransactionsArray.length, mapIndex.size);
    addTableRow(opType, timeVec, timeMap, mem.vecBytes, mem.mapBytes);
}

function addTableRow(op, vecTime, mapTime, vecMem, mapMem) {
    const empty = document.getElementById('empty-row');
    if (empty) empty.remove();

    const tbody = document.getElementById('table-body');
    const tr = document.createElement('tr');

    const fmtT = (t, o) => t < o ? `<span class="winner">${t.toFixed(4)} ms</span>` : `<span class="loser">${t.toFixed(4)} ms</span>`;
    const fmtM = (m, o) => m <= o ? `<span class="winner">${m.toLocaleString()} B</span>` : `<span class="loser">${m.toLocaleString()} B</span>`;

    tr.innerHTML = `
        <td style="text-transform:capitalize;"><strong>${op}</strong></td>
        <td>${fmtT(vecTime, mapTime)}</td>
        <td>${fmtT(mapTime, vecTime)}</td>
        <td>${fmtM(vecMem, mapMem)}</td>
        <td>${fmtM(mapMem, vecMem)}</td>
        <td><button class="btn btn-small" onclick="visualize('${op}', ${vecTime}, ${mapTime})">Lihat</button></td>
    `;
    tbody.appendChild(tr);
    visualize(op, vecTime, mapTime);
}

function visualize(operation, vecTime, mapTime) {
    const sec = document.getElementById('visualization-section');
    sec.style.display = 'block';
    sec.scrollIntoView({ behavior: 'smooth' });

    document.getElementById('current-op').textContent = operation.toUpperCase();
    const vBar = document.getElementById('vector-bar');
    const mBar = document.getElementById('map-bar');
    
    // reset
    vBar.style.width = '0%';
    mBar.style.width = '0%';
    
    setTimeout(() => {
        const maxMs = Math.max(vecTime, mapTime);
        let vPct = (vecTime / maxMs) * 100;
        let mPct = (mapTime / maxMs) * 100;
        if (vPct < 3) vPct = 3; if (mPct < 3) mPct = 3;

        vBar.style.width = `${vPct}%`;
        mBar.style.width = `${mPct}%`;
        
        document.getElementById('vector-val').textContent = vecTime.toFixed(4) + ' ms';
        document.getElementById('map-val').textContent = mapTime.toFixed(4) + ' ms';

        let win = vecTime < mapTime ? 'Vector' : 'Map';
        document.getElementById('verdict-box').innerHTML = `Untuk <strong>${operation}</strong>, <strong>${win}</strong> lebih cepat dalam simulasi memori ini.`;
    }, 100);
}
