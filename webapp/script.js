let products = [];
let vectorTransactions = []; // Array to simulate std::vector
let mapTransactionsArray = []; // Base array for Hash Map
let mapIndex = new Map(); // Simulating unordered_map<string, array_of_indexes>

// Utils
function parseCSV(text, expectedHeaders) {
    const lines = text.trim().split('\n');
    let firstLine = lines[0].split(',').map(h => h.trim());
    let headers, startIndex;
    
    if (firstLine[0].match(/^[PT]\d+/)) {
        headers = expectedHeaders;
        startIndex = 0;
    } else {
        headers = firstLine;
        startIndex = 1;
    }
    
    return lines.slice(startIndex).map(line => {
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
        products = parseCSV(e.target.result, ['productId', 'productName', 'category']);
        
        const reader2 = new FileReader();
        reader2.onload = (ev) => {
            const txData = parseCSV(ev.target.result, ['transactionId', 'customerId', 'date', 'productId', 'productName', 'category', 'quantity']);
            
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
    let resultData = null;
    
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
            if (k === 0) resultData = res;
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
    } else if (opType === 'search_cust') {
        const custId = document.getElementById('search-cust-id').value;
        if (!custId) return alert("Isi Customer ID");
        const vStart = performance.now();
        for(let k=0; k<REPEAT_COUNT; k++) {
            let res = [];
            for (let i = 0; i < vectorTransactions.length; i++) {
                if (vectorTransactions[i].customerId === custId) res.push(vectorTransactions[i]);
            }
        }
        timeVec = performance.now() - vStart;
        const mStart = performance.now();
        for(let k=0; k<REPEAT_COUNT; k++) {
            let res = [];
            for (let i = 0; i < mapTransactionsArray.length; i++) {
                if (mapTransactionsArray[i].customerId === custId) res.push(mapTransactionsArray[i]);
            }
            if (k === 0) resultData = res;
        }
        timeMap = performance.now() - mStart;
    } else if (opType === 'search_prod') {
        const prodId = document.getElementById('search-prod-id').value;
        if (!prodId) return alert("Isi Product ID");
        const vStart = performance.now();
        for(let k=0; k<REPEAT_COUNT; k++) {
            let res = [];
            for (let i = 0; i < vectorTransactions.length; i++) {
                if (vectorTransactions[i].productId === prodId) res.push(vectorTransactions[i]);
            }
        }
        timeVec = performance.now() - vStart;
        const mStart = performance.now();
        for(let k=0; k<REPEAT_COUNT; k++) {
            let res = [];
            for (let i = 0; i < mapTransactionsArray.length; i++) {
                if (mapTransactionsArray[i].productId === prodId) res.push(mapTransactionsArray[i]);
            }
            if (k === 0) resultData = res;
        }
        timeMap = performance.now() - mStart;
    } else if (opType === 'top_n') {
        const nVal = parseInt(document.getElementById('top-n-val').value) || 5;
        const vStart = performance.now();
        for(let k=0; k<20; k++) { 
            let freqs = [];
            for (let i = 0; i < vectorTransactions.length; i++) {
                let pid = vectorTransactions[i].productId;
                let found = false;
                for(let j=0; j<freqs.length; j++){
                    if(freqs[j].id === pid) { freqs[j].count++; found = true; break; }
                }
                if(!found) freqs.push({id: pid, count: 1});
            }
            freqs.sort((a,b) => b.count - a.count);
            let top = freqs.slice(0, nVal);
        }
        timeVec = performance.now() - vStart;
        const mStart = performance.now();
        for(let k=0; k<20; k++) {
            let freqMap = new Map();
            for (let i = 0; i < mapTransactionsArray.length; i++) {
                let pid = mapTransactionsArray[i].productId;
                freqMap.set(pid, (freqMap.get(pid) || 0) + 1);
            }
            let freqs = Array.from(freqMap, ([id, count]) => ({id, count}));
            freqs.sort((a,b) => b.count - a.count);
            let top = freqs.slice(0, nVal);
            if (k === 0) resultData = top;
        }
        timeMap = performance.now() - mStart;
    } else if (opType === 'fbt') {
        const vStart = performance.now();
        for(let k=0; k<5; k++) {
            let txGroups = [];
            for(let i=0; i<vectorTransactions.length; i++) {
                let tid = vectorTransactions[i].transactionId;
                let pid = vectorTransactions[i].productId;
                let found = false;
                for(let j=0; j<txGroups.length; j++){
                    if(txGroups[j].tx === tid) { txGroups[j].prods.push(pid); found = true; break; }
                }
                if(!found) txGroups.push({tx: tid, prods: [pid]});
            }
            let pairFreq = [];
            for(let g=0; g<txGroups.length; g++) {
                let prods = txGroups[g].prods;
                for(let i=0; i<prods.length; i++){
                    for(let j=i+1; j<prods.length; j++){
                        let p1 = prods[i], p2 = prods[j];
                        if(p1 > p2) { let temp = p1; p1=p2; p2=temp; }
                        let pairId = p1 + "-" + p2;
                        let found = false;
                        for(let x=0; x<pairFreq.length; x++){
                            if(pairFreq[x].id === pairId) { pairFreq[x].count++; found = true; break;}
                        }
                        if(!found) pairFreq.push({id: pairId, count: 1});
                    }
                }
            }
        }
        timeVec = performance.now() - vStart;
        const mStart = performance.now();
        for(let k=0; k<5; k++) {
            let txMap = new Map();
            for(let i=0; i<mapTransactionsArray.length; i++) {
                let tid = mapTransactionsArray[i].transactionId;
                let pid = mapTransactionsArray[i].productId;
                if(!txMap.has(tid)) txMap.set(tid, []);
                txMap.get(tid).push(pid);
            }
            let pairMap = new Map();
            for (let [tid, prods] of txMap.entries()) {
                for(let i=0; i<prods.length; i++){
                    for(let j=i+1; j<prods.length; j++){
                        let p1 = prods[i], p2 = prods[j];
                        if(p1 > p2) { let temp = p1; p1=p2; p2=temp; }
                        let pairId = p1 + "-" + p2;
                        pairMap.set(pairId, (pairMap.get(pairId) || 0) + 1);
                    }
                }
            }
            if (k === 0) {
                resultData = Array.from(pairMap, ([id, count]) => ({id, count})).sort((a,b) => b.count - a.count).slice(0, 10);
            }
        }
        timeMap = performance.now() - mStart;
    }

    // Display
    const mem = estimateMemory(vectorTransactions.length, mapTransactionsArray.length, mapIndex.size);
    addTableRow(opType, timeVec, timeMap, mem.vecBytes, mem.mapBytes);
    
    // Show Visual Result Data
    if (resultData && resultData.length > 0) {
        showVisualResult(opType, resultData);
    } else if (resultData && resultData.length === 0) {
        openModal("Hasil " + opType, "Tidak ada data yang ditemukan.");
    } else if (opType === 'insert' || opType === 'update' || opType === 'delete') {
        openModal("Hasil " + opType, "Operasi berhasil dijalankan.");
    }
}

function showVisualResult(opType, data) {
    let text = "";
    if (opType === 'search' || opType === 'search_cust' || opType === 'search_prod') {
        text = "TransactionID | CustomerID | ProductID | Quantity\n";
        text += "--------------------------------------------------\n";
        data.forEach(t => {
            text += `${t.transactionId} | ${t.customerId} | ${t.productId} | ${t.quantity}\n`;
        });
    } else if (opType === 'top_n') {
        text = "ProductID | Total Terjual\n";
        text += "-------------------------\n";
        data.forEach(t => {
            text += `${t.id} | ${t.count}\n`;
        });
    } else if (opType === 'fbt') {
        text = "Pasangan Produk | Frekuensi Muncul Bersama\n";
        text += "------------------------------------------\n";
        data.forEach(t => {
            text += `${t.id} | ${t.count}\n`;
        });
    }
    openModal("Hasil " + opType.toUpperCase(), text);
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
        <td><button class="btn btn-small" onclick="visualize('${op}', ${vecTime}, ${mapTime}, ${vecMem}, ${mapMem})">Lihat</button></td>
    `;
    tbody.appendChild(tr);
    visualize(op, vecTime, mapTime, vecMem, mapMem);
}

function visualize(operation, vecTime, mapTime, vecMem, mapMem) {
    const sec = document.getElementById('visualization-section');
    sec.style.display = 'block';
    sec.scrollIntoView({ behavior: 'smooth' });

    document.getElementById('current-op').textContent = operation.toUpperCase();
    const vBar = document.getElementById('vector-bar');
    const mBar = document.getElementById('map-bar');
    const vMemBar = document.getElementById('vector-mem-bar');
    const mMemBar = document.getElementById('map-mem-bar');
    
    // reset
    vBar.style.width = '0%';
    mBar.style.width = '0%';
    vMemBar.style.width = '0%';
    mMemBar.style.width = '0%';
    
    setTimeout(() => {
        const maxMs = Math.max(vecTime, mapTime);
        let vPct = maxMs === 0 ? 0 : (vecTime / maxMs) * 100;
        let mPct = maxMs === 0 ? 0 : (mapTime / maxMs) * 100;
        if (vPct > 0 && vPct < 3) vPct = 3; if (mPct > 0 && mPct < 3) mPct = 3;

        vBar.style.width = `${vPct}%`;
        mBar.style.width = `${mPct}%`;
        
        document.getElementById('vector-val').textContent = vecTime.toFixed(4) + ' ms';
        document.getElementById('map-val').textContent = mapTime.toFixed(4) + ' ms';

        const maxMem = Math.max(vecMem, mapMem);
        let vMemPct = maxMem === 0 ? 0 : (vecMem / maxMem) * 100;
        let mMemPct = maxMem === 0 ? 0 : (mapMem / maxMem) * 100;
        if (vMemPct > 0 && vMemPct < 3) vMemPct = 3; if (mMemPct > 0 && mMemPct < 3) mMemPct = 3;

        vMemBar.style.width = `${vMemPct}%`;
        mMemBar.style.width = `${mMemPct}%`;
        
        document.getElementById('vector-mem-val').textContent = vecMem.toLocaleString() + ' B';
        document.getElementById('map-mem-val').textContent = mapMem.toLocaleString() + ' B';

        let winTime = vecTime <= mapTime ? 'Vector' : 'Map';
        let winMem = vecMem <= mapMem ? 'Vector' : 'Map';
        document.getElementById('verdict-box').innerHTML = `Untuk <strong>${operation}</strong>:<br>⏱️ Waktu: <strong>${winTime}</strong> lebih cepat.<br>💾 Memori: <strong>${winMem}</strong> lebih efisien.`;
    }, 100);
}

// Utilities for showing modal and downloading
function openModal(title, content) {
    document.getElementById('modal-title').textContent = title;
    document.getElementById('modal-body').textContent = content;
    document.getElementById('data-modal').style.display = 'flex';
}

function showAllTransactions() {
    if(vectorTransactions.length === 0) return alert("Belum ada data.");
    let text = "TransactionID | CustomerID | ProductID | Quantity\n";
    text += "--------------------------------------------------\n";
    let limit = Math.min(vectorTransactions.length, 100);
    for(let i=0; i<limit; i++) {
        let t = vectorTransactions[i];
        text += `${t.transactionId} | ${t.customerId} | ${t.productId} | ${t.quantity}\n`;
    }
    if (vectorTransactions.length > limit) text += `\n... dan ${vectorTransactions.length - limit} data lainnya.`;
    openModal("Semua Transaksi (Preview)", text);
}

function showAllProducts() {
    if(products.length === 0) return alert("Belum ada data master produk.");
    let text = "ProductID | Name | Price | Category\n";
    text += "--------------------------------------\n";
    let limit = Math.min(products.length, 100);
    for(let i=0; i<limit; i++) {
        let p = products[i];
        let name = p.name || p.productName || "";
        let price = p.price || "";
        let cat = p.category || "";
        text += `${p.productId} | ${name} | ${price} | ${cat}\n`;
    }
    if (products.length > limit) text += `\n... dan ${products.length - limit} data lainnya.`;
    openModal("Master Produk (Preview)", text);
}

function saveTransactions() {
    if(vectorTransactions.length === 0) return alert("Belum ada data.");
    let csvContent = "data:text/csv;charset=utf-8,";
    // Check if there are keys in the first object, otherwise use default
    let keys = Object.keys(vectorTransactions[0]);
    csvContent += keys.join(",") + "\n";
    vectorTransactions.forEach(t => {
        let values = keys.map(k => t[k]);
        csvContent += values.join(",") + "\n";
    });
    var encodedUri = encodeURI(csvContent);
    var link = document.createElement("a");
    link.setAttribute("href", encodedUri);
    link.setAttribute("download", "transactions_saved.csv");
    document.body.appendChild(link);
    link.click();
    link.remove();
}

