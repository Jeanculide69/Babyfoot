#ifndef WEB_PAGES_H
#define WEB_PAGES_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Babyfoot Force - Console</title>
    <style>
        :root { --jedi: #58a6ff; --sith: #f85149; --gold: #ffe000; --bg: #0d1117; --glass: rgba(22, 27, 34, 0.8); }
        body { background: var(--bg); color: #c9d1d9; font-family: sans-serif; margin: 0; padding: 20px; display: flex; flex-direction: column; align-items: center; }
        .container { max-width: 500px; width: 100%; }
        .card { background: var(--glass); backdrop-filter: blur(10px); border: 1px solid #30363d; border-radius: 20px; padding: 25px; margin-bottom: 20px; box-shadow: 0 8px 32px rgba(0,0,0,0.5); }
        h2 { color: var(--gold); text-transform: uppercase; font-size: 1.1rem; margin-top: 0; border-bottom: 1px solid #333; padding-bottom: 10px; }
        .score-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; }
        .score-val { font-size: 64px; font-weight: 900; margin: 10px 0; }
        .jedi { color: var(--jedi); text-shadow: 0 0 15px var(--jedi); }
        .sith { color: var(--sith); text-shadow: 0 0 15px var(--sith); }
        .btn { display: block; padding: 12px; background: #21262d; border: 1px solid #30363d; border-radius: 10px; color: white; text-decoration: none; font-weight: bold; text-align: center; cursor: pointer; margin: 5px 0; user-select: none; -webkit-user-select: none; }
        .btn:active { transform: scale(0.95); }
        .btn-jedi { border-color: var(--jedi); color: var(--jedi); }
        .btn-sith { border-color: var(--sith); color: var(--sith); }
        .btn-gold { background: var(--gold); color: black; font-weight: 900; }
        .btn-gold:hover { background: #fff; box-shadow: 0 0 20px var(--gold); }
        .btn-test { background: #21262d; border-color: #30363d; font-size: 10px; padding: 8px 5px; }
        .btn-test:hover { background: #30363d; border-color: #8b949e; }
        .test-grid { display: grid; grid-template-columns: repeat(4, 1fr); gap: 8px; }
        .slider-card { background: rgba(255,255,255,0.05); padding: 12px; border-radius: 10px; margin-top: 10px; }
        .action-row { display: flex; gap: 10px; margin-top: 10px; }
        .action-btn { flex: 1; font-size: 14px; border-width: 2px; }
        .slider-card { margin-top: 15px; text-align: left; }
        .slider-label { font-size: 10px; color: #8b949e; text-transform: uppercase; margin-bottom: 5px; display: block; }
        input[type=range] { width: 100%; height: 8px; border-radius: 5px; background: #30363d; outline: none; -webkit-appearance: none; margin: 10px 0; }
        input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; width: 20px; height: 20px; border-radius: 50%; background: var(--gold); cursor: pointer; box-shadow: 0 0 10px var(--gold); }
    </style>
</head>
<body>
    <div class="container">
        <div class="card">
            <h2>TELEMETRIE LIVE</h2>
            <div class="score-grid">
                <div style="text-align:center;">
                    <div id="t1_name" style="color:var(--jedi)">JEDI</div>
                    <div id="s1" class="score-val jedi">0</div>
                    <div class="action-row">
                        <div class="btn btn-jedi action-btn" onclick="doAct('M1')">-</div>
                        <div class="btn btn-jedi action-btn" onclick="doAct('P1')">+</div>
                    </div>
                </div>
                <div style="text-align:center;">
                    <div id="t2_name" style="color:var(--sith)">SITH</div>
                    <div id="s2" class="score-val sith">0</div>
                    <div class="action-row">
                        <div class="btn btn-sith action-btn" onclick="doAct('M2')">-</div>
                        <div class="btn btn-sith action-btn" onclick="doAct('P2')">+</div>
                    </div>
                </div>
            </div>
            <div style="text-align:center; margin-top:15px; color:var(--gold)">BALLES RESTANTES: <span id="ball">11</span></div>
        </div>

        <div class="card">
            <h2>CENTRE DE COMMANDE</h2>
            <div class="score-grid">
                <div>
                    <div class="btn btn-jedi" onclick="doAct('B1')">BUT JEDI</div>
                    <div class="btn btn-jedi" onclick="doAct('G1')">GAMELLE</div>
                </div>
                <div>
                    <div class="btn btn-sith" onclick="doAct('B2')">BUT SITH</div>
                    <div class="btn btn-sith" onclick="doAct('G2')">GAMELLE</div>
                </div>
            </div>
            <div class="btn btn-gold" style="margin-top:15px;" onclick="doAct('DEMI')">DEMI (ANNULER DERNIER BUT)</div>
            <div class="btn btn-gold" style="margin-top:5px;" onclick="doAct('OK')">START / RESET / OK</div>
            
            <div class="slider-card">
                <span class="slider-label">Luminosite Matrice LED</span>
                <input type="range" min="10" max="255" value="100" onchange="doSlider('BR', this.value)">
            </div>
            <div class="slider-card">
                <span class="slider-label">Luminosite Ambilight</span>
                <input type="range" min="0" max="255" value="150" onchange="doSlider('AB', this.value)">
            </div>
            <div class="slider-card">
                <span class="slider-label">Volume Audio</span>
                <input type="range" min="0" max="30" value="25" onchange="doSlider('VL', this.value)">
            </div>
        </div>

        <div class="card" style="border-color: #444; background: rgba(255,255,255,0.02)">
            <h2 style="color: #8b949e; font-size: 16px;">MODE TEST</h2>
            <div style="font-size: 10px; color: #8b949e; margin-bottom: 8px; text-transform: uppercase; letter-spacing: 1px;">Animations</div>
            <div class="test-grid">
                <div class="btn btn-test" onclick="testAnim(1)">BUT BLEU</div>
                <div class="btn btn-test" onclick="testAnim(2)">BUT ROUGE</div>
                <div class="btn btn-test" onclick="testAnim(3)">GAM. BLEU</div>
                <div class="btn btn-test" onclick="testAnim(4)">GAM. ROUGE</div>
                <div class="btn btn-test" onclick="testAnim(5)">BIERE</div>
                <div class="btn btn-test" onclick="testAnim(6)">VIC. BLEU</div>
                <div class="btn btn-test" onclick="testAnim(7)">VIC. ROUGE</div>
                <div class="btn btn-test" onclick="testAnim(8)">BALLE MATCH</div>
                <div class="btn btn-test" onclick="testAnim(9)">DEMI</div>
            </div>
            <div style="font-size: 10px; color: #8b949e; margin: 12px 0 8px; text-transform: uppercase; letter-spacing: 1px;">Sons (SFX)</div>
            <div class="test-grid">
                <div class="btn btn-test" onclick="testSFX(1)">INTRO</div>
                <div class="btn btn-test" onclick="testSFX(7)">AMBIANCE</div>
                <div class="btn btn-test" onclick="testSFX(2)">SFX 2</div>
                <div class="btn btn-test" onclick="testSFX(3)">SFX 3</div>
                <div class="btn btn-test" onclick="testSFX(4)">SFX 4</div>
                <div class="btn btn-test" onclick="testSFX(5)">SFX 5</div>
                <div class="btn btn-test" onclick="testSFX(6)">SFX 6</div>
            </div>
        </div>

        <div class="nav-grid" style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px;">
            <a href="/tournament" class="btn">TOURNOI</a>
            <a href="/tv" class="btn">TV LIVE</a>
            <a href="/logs" class="btn">LOGS</a>
            <a href="/wifi" class="btn">WIFI</a>
            <a href="/files" class="btn" style="color:var(--gold); border-color:var(--gold);">FICHIERS</a>
            <a href="/update" class="btn">MAJ OTA</a>
        </div>
        <a href="javascript:doAct('REBOOT')" class="btn" style="background:#d73a49; margin-top:10px;">REBOOT ESP32</a>
    </div>
    <script>
        function doAct(id){ 
            console.log("Action sent: " + id);
            fetch('/action?id='+id).catch(e => console.error("Action fail:", e)); 
        }
        function doSlider(id, val){ fetch('/action?id='+id+'&val='+val); }
        function testSFX(id){ fetch('/api/test_sfx?id='+id); }
        function testAnim(id){ fetch('/api/test_anim?id='+id); }
        function update() {
            fetch('/api/status').then(r => r.json()).then(data => {
                document.getElementById('s1').innerText = data.p1;
                document.getElementById('s2').innerText = data.p2;
                document.getElementById('ball').innerText = data.ball;
                document.getElementById('t1_name').innerText = data.t1;
                document.getElementById('t2_name').innerText = data.t2;
                if(typeof syncResults === 'function') syncResults(data); 
            });
        }
        setInterval(update, 3000); 
        update();
    </script>
</body>
</html>
)rawliteral";

const char FILES_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Gestionnaire de Fichiers</title>
    <style>
        :root { --gold: #ffe000; --bg: #0d1117; --card: #161b22; --danger: #f85149; }
        body { background: var(--bg); color: #c9d1d9; font-family: sans-serif; margin: 0; padding: 20px; display: flex; flex-direction: column; align-items: center; }
        .container { max-width: 600px; width: 100%; }
        .card { background: var(--card); border: 1px solid #30363d; border-radius: 15px; padding: 25px; margin-bottom: 20px; }
        h1 { color: var(--gold); text-transform: uppercase; font-size: 1.2rem; margin-top: 0; border-bottom: 1px solid #333; padding-bottom: 10px; display: flex; justify-content: space-between; }
        .file-list { margin-top: 15px; border-radius: 10px; overflow: hidden; border: 1px solid #30363d; }
        .file-item { display: flex; justify-content: space-between; align-items: center; padding: 12px 15px; background: rgba(255,255,255,0.02); border-bottom: 1px solid #30363d; }
        .file-item:last-child { border-bottom: none; }
        .file-item:hover { background: rgba(255,255,255,0.05); }
        .file-name { font-weight: bold; color: #58a6ff; font-family: monospace; }
        .file-size { color: #8b949e; font-size: 0.9em; margin-right: 15px; }
        .btn { padding: 8px 15px; background: #21262d; border: 1px solid #30363d; border-radius: 6px; color: white; cursor: pointer; text-decoration: none; font-weight: bold; }
        .btn:hover { background: #30363d; }
        .btn-danger { color: var(--danger); border-color: var(--danger); background: transparent; padding: 5px 10px; }
        .btn-danger:hover { background: var(--danger); color: white; }
        .btn-gold { background: var(--gold); color: black; padding: 12px 20px; border: none; font-size: 1.1em; width: 100%; border-radius: 10px; }
        .dropzone { border: 2px dashed #30363d; border-radius: 10px; padding: 40px 20px; text-align: center; color: #8b949e; margin-bottom: 20px; cursor: pointer; transition: 0.3s; }
        .dropzone.dragover { border-color: var(--gold); background: rgba(255,224,0,0.1); color: var(--gold); }
        .progress-bar { width: 100%; height: 10px; background: #30363d; border-radius: 5px; overflow: hidden; margin-top: 10px; display: none; }
        .progress-fill { height: 100%; background: var(--gold); width: 0%; transition: 0.2s; }
        #status { text-align: center; margin-top: 10px; font-weight: bold; color: var(--gold); }
    </style>
</head>
<body>
    <div class="container">
        <button class="btn" style="margin-bottom:20px;" onclick="location.href='/'">⬅ RETOUR CONSOLE</button>
        
        <div class="card">
            <h1>UPLOADER UN FICHIER</h1>
            <div class="dropzone" id="dropzone" onclick="document.getElementById('file-input').click()">
                <div style="font-size: 2em; margin-bottom: 10px;">📁</div>
                Glissez-déposez vos fichiers GIF ici<br>ou cliquez pour parcourir
            </div>
            <input type="file" id="file-input" style="display:none;" multiple accept=".gif,.json">
            <div class="progress-bar" id="progress-bar"><div class="progress-fill" id="progress-fill"></div></div>
            <div id="status"></div>
        </div>

        <div class="card">
            <h1>FICHIERS SUR L'ESP32 <span style="font-size:0.7em; color:#8b949e" id="fs-usage"></span></h1>
            <div class="file-list" id="file-list">
                <div style="padding:20px; text-align:center; color:#8b949e">Chargement des fichiers...</div>
            </div>
            <button class="btn btn-danger" style="width:100%; margin-top:15px; padding:10px;" onclick="formatFS()">⚠️ FORMATER LA MEMOIRE (TOUT EFFACER)</button>
        </div>
    </div>

    <script>
        const dropzone = document.getElementById('dropzone');
        const fileInput = document.getElementById('file-input');
        const fileList = document.getElementById('file-list');
        const progressBar = document.getElementById('progress-bar');
        const progressFill = document.getElementById('progress-fill');
        const statusEl = document.getElementById('status');

        // Drag & Drop
        dropzone.addEventListener('dragover', (e) => { e.preventDefault(); dropzone.classList.add('dragover'); });
        dropzone.addEventListener('dragleave', () => dropzone.classList.remove('dragover'));
        dropzone.addEventListener('drop', (e) => {
            e.preventDefault();
            dropzone.classList.remove('dragover');
            handleFiles(e.dataTransfer.files);
        });
        fileInput.addEventListener('change', () => handleFiles(fileInput.files));

        function handleFiles(files) {
            if (files.length === 0) return;
            uploadFile(files[0], 0, files);
        }

        function uploadFile(file, index, files) {
            if (index >= files.length) {
                statusEl.innerText = "Upload terminé !";
                setTimeout(() => { progressBar.style.display = 'none'; statusEl.innerText = ""; }, 3000);
                loadFiles();
                return;
            }

            statusEl.innerText = `Envoi de ${file.name} (${index + 1}/${files.length})...`;
            progressBar.style.display = 'block';
            progressFill.style.width = '0%';

            const formData = new FormData();
            formData.append('update', file, file.name);

            const xhr = new XMLHttpRequest();
            xhr.open('POST', '/api/upload_gif', true);

            xhr.upload.onprogress = (e) => {
                if (e.lengthComputable) {
                    const percent = (e.loaded / e.total) * 100;
                    progressFill.style.width = percent + '%';
                }
            };

            xhr.onload = () => {
                if (xhr.status === 200) {
                    uploadFile(files[index + 1], index + 1, files); // Fichier suivant
                } else {
                    statusEl.innerText = `Erreur lors de l'envoi de ${file.name}`;
                    statusEl.style.color = 'var(--danger)';
                }
            };
            xhr.onerror = () => { statusEl.innerText = "Erreur de connexion"; statusEl.style.color = 'var(--danger)'; };
            xhr.send(formData);
        }

        function loadFiles() {
            fetch('/api/list_files').then(r => r.json()).then(data => {
                if (data.length === 0) {
                    fileList.innerHTML = '<div style="padding:20px; text-align:center; color:#8b949e">Aucun fichier trouvé.</div>';
                    document.getElementById('fs-usage').innerText = "0 KB";
                    return;
                }
                
                let totalSize = 0;
                let html = '';
                // Trier par ordre alphabétique
                data.sort((a,b) => a.n.localeCompare(b.n));
                
                data.forEach(f => {
                    totalSize += f.s;
                    const sizeKB = (f.s / 1024).toFixed(1);
                    html += `
                        <div class="file-item">
                            <div>
                                <div class="file-name">${f.n}</div>
                            </div>
                            <div style="display:flex; align-items:center;">
                                <div class="file-size ${f.s == 0 ? 'file-empty' : ''}" style="${f.s == 0 ? 'color:var(--danger); font-weight:bold;' : ''}">
                                    ${f.s == 0 ? '0.0 KB (VIDE!)' : sizeKB + ' KB'}
                                </div>
                                <button class="btn btn-danger" onclick="deleteFile('${f.n}')">X</button>
                            </div>
                        </div>
                    `;
                });
                fileList.innerHTML = html;
                document.getElementById('fs-usage').innerText = `${(totalSize / 1024).toFixed(1)} KB Total`;
            });
        }

        function deleteFile(name) {
            if(confirm(`Voulez-vous vraiment supprimer le fichier ${name} ?`)) {
                fetch('/api/delete_file', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `filename=${encodeURIComponent(name)}`
                }).then(r => {
                    if(r.ok) loadFiles();
                    else alert("Erreur lors de la suppression.");
                });
            }
        }

        function formatFS() {
            if(confirm("ATTENTION : Cela va supprimer TOUS les fichiers (GIFs, WiFi, Tournoi). Etes-vous sûr ?")) {
                statusEl.innerText = "Formatage en cours... Patientez...";
                fetch('/api/format_fs').then(r => {
                    alert("Mémoire formatée. L'ESP32 va redémarrer.");
                    location.href = '/';
                });
            }
        }

        loadFiles();
    </script>
</body>
</html>
)rawliteral";

const char TV_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8"><title>Star Wars - Stadium TV</title>
    <style>
        :root { --gold: #ffe000; --jedi: #00f2ff; --sith: #ff0000; --bg: #05070a; --glass: rgba(22, 27, 34, 0.8); }
        body { background: var(--bg); color: white; font-family: 'Orbitron', sans-serif; margin: 0; padding: 20px; overflow: hidden; display: flex; flex-direction: column; height: 100vh; box-sizing: border-box; }
        .header { display: flex; justify-content: space-between; align-items: center; border-bottom: 2px solid var(--gold); padding-bottom: 10px; margin-bottom: 10px; }
        .match-info { font-size: 1.2vw; color: var(--gold); text-transform: uppercase; letter-spacing: 2px; }
        .timer { font-size: 2.5vw; font-weight: bold; font-family: monospace; }
        .scoreboard { display: flex; align-items: center; justify-content: space-around; height: 40vh; margin-bottom: 20px; }
        .team-box { text-align: center; width: 40%; }
        .team-name { font-size: 3vw; margin-bottom: 10px; text-transform: uppercase; letter-spacing: 5px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
        .score-val { font-size: 12vw; font-weight: 900; line-height: 1; text-shadow: 0 0 30px currentColor; border: 6px solid currentColor; padding: 10px 30px; border-radius: 20px; display: inline-block; min-width: 15vw; }
        .demi-zone { margin-top: 10px; height: 30px; }
        .demi-label { color: var(--gold); font-size: 1.2vw; font-weight: bold; text-transform: uppercase; letter-spacing: 3px; animation: blink 1s infinite; display: none; text-shadow: 0 0 10px var(--gold); }
        @keyframes blink { 0% { opacity: 1; } 50% { opacity: 0.2; } 100% { opacity: 1; } }

        .ball-count { text-align: center; color: var(--gold); }
        .ball-val { font-size: 5vw; font-weight: bold; }

        .bottom-grid { flex: 1; display: grid; grid-template-columns: 400px 1fr; gap: 20px; overflow: hidden; margin-top: 20px; }
        .logs-panel { background: var(--glass); border: 1px solid #444; border-radius: 15px; padding: 20px; display: flex; flex-direction: column; overflow: hidden; box-shadow: 0 0 20px rgba(0,0,0,0.5); }
        @keyframes flash {
            0% { background-color: rgba(255, 215, 0, 0.5); transform: scale(1.02); }
            100% { background-color: rgba(255,255,255,0.05); transform: scale(1); }
        }
        .log-entry { font-size: 1.1vw; border-bottom: 1px solid #222; padding: 12px 0; color: #aaa; font-family: monospace; animation: flash 0.8s ease-out; }
        .bracket-panel { background: var(--glass); border: 1px solid #444; border-radius: 15px; padding: 20px; display: flex; flex-direction: column; overflow: hidden; }
        .bracket-scroll { flex: 1; display: flex; align-items: center; justify-content: center; gap: 50px; }
        .round { display: flex; flex-direction: column; gap: 25px; align-items: center; }
        .match { background: #111; border: 1px solid #333; padding: 15px; border-radius: 12px; min-width: 200px; box-shadow: 4px 4px 10px rgba(0,0,0,0.3); }
        .team { display: flex; justify-content: space-between; font-size: 1.4vw; padding: 5px 0; border-bottom: 1px solid #222; }
        .score { color: var(--gold); font-weight: bold; margin-left: 15px; }
        .winner { color: var(--gold); text-shadow: 0 0 10px var(--gold); }
        .active { border-color: var(--gold); box-shadow: 0 0 15px var(--gold); }
        .overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.9); z-index: 1000; display: none; flex-direction: column; align-items: center; justify-content: center; text-align: center; font-family: 'Orbitron', sans-serif; }
        .overlay h1 { font-size: 5vw; color: var(--gold); text-shadow: 0 0 20px var(--gold); margin: 0; }
        .overlay .winner-name { font-size: 8vw; color: white; margin: 20px 0; text-transform: uppercase; }
        .overlay .quote { font-size: 2vw; color: #888; font-style: italic; }
        
        /* Nouveaux styles pour les alertes Flash */
        .flash-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; z-index: 2000; display: none; align-items: center; justify-content: center; text-align: center; pointer-events: none; }
        .flash-content { transform: scale(0.5); opacity: 0; transition: all 0.3s cubic-bezier(0.175, 0.885, 0.32, 1.275); }
        .flash-active .flash-content { transform: scale(1.2); opacity: 1; }
        .flash-title { font-size: 10vw; font-weight: 900; text-transform: uppercase; letter-spacing: 10px; margin: 0; }
        .flash-subtitle { font-size: 3vw; color: white; text-transform: uppercase; letter-spacing: 5px; }
        
        .goal-blue { background: radial-gradient(circle, rgba(0,242,255,0.4) 0%, rgba(0,0,0,0.9) 80%); }
        .goal-red { background: radial-gradient(circle, rgba(255,0,0,0.4) 0%, rgba(0,0,0,0.9) 80%); }
        .goal-gold { background: radial-gradient(circle, rgba(255,224,0,0.4) 0%, rgba(0,0,0,0.9) 80%); }

    </style>
    <link href="https://fonts.googleapis.com/css2?family=Orbitron:wght@400;900&display=swap" rel="stylesheet">
</head>
<body>
    <div class="header">
        <div style="display:flex; gap:20px; align-items:center;">
            <button onclick="location.href='/'" style="background:var(--gold); border:none; padding:10px 20px; cursor:pointer; font-weight:bold; border-radius:5px;">RETOUR</button>
            <button onclick="fetch('/action?id=NEXT_MATCH')" style="background:#2ea043; color:white; border:none; padding:10px 20px; cursor:pointer; font-weight:bold; border-radius:5px;">MATCH SUIVANT</button>
            <div class="match-info">DIRECT STADIUM - CHAMPIONNAT</div>
        </div>
        <div class="timer" id="timer">00:00</div>
        <div class="match-info" id="match-label">MATCH EN COURS</div>
    </div>
    <div class="scoreboard">
        <div class="team-box" style="color:var(--jedi)"><div class="team-name" id="t1_name">JEDI</div><div class="score-val" id="s1">0</div></div>
        <div class="ball-count">
            <div style="font-size: 1vw; letter-spacing: 3px;">BALLES</div>
            <div class="ball-val" id="ball">11</div>
            <div class="demi-zone"><div id="demi-global" class="demi-label">⚠ POINT EN ATTENTE</div></div>
        </div>
        <div class="team-box" style="color:var(--sith)"><div class="team-name" id="t2_name">SITH</div><div class="score-val" id="s2">0</div></div>
    </div>


    <div class="bottom-grid">
        <div class="logs-panel"><div style="color:var(--gold); font-size:1vw; margin-bottom:10px; font-weight:bold;">EVENEMENTS RECENTS</div><div id="log-box" style="overflow-y:auto; flex:1;"></div></div>
        <div class="bracket-panel"><div style="color:var(--gold); font-size:1vw; margin-bottom:10px; font-weight:bold;">TABLEAU DU TOURNOI</div><div class="bracket-scroll" id="bracket-box"></div></div>
    </div>
    <div id="overlay" class="overlay"></div>
    <div id="flash" class="flash-overlay"><div class="flash-content"><h2 class="flash-title" id="flash-title">BUT !</h2><p class="flash-subtitle" id="flash-sub">LA FORCE EST AVEC VOUS</p></div></div>
    
    <div style="position:fixed; bottom:20px; right:20px; z-index:1000;">
        <button onclick="fetch('/action?id=OK')" style="background:var(--gold); color:black; font-weight:bold; padding:15px 30px; border:none; border-radius:10px; cursor:pointer; font-size:1.5vw; box-shadow: 0 0 20px rgba(0,0,0,0.5);">BOUTON OK</button>
    </div>
    <script>
        let tournament = { teams: [], rounds: [], activeMatch: null };
        let startTime = Date.now();
        let isRunning = false;

        function showFlash(type, title, sub) {
            const f = document.getElementById('flash');
            const ft = document.getElementById('flash-title');
            const fs = document.getElementById('flash-sub');
            
            f.className = 'flash-overlay ' + type;
            ft.innerText = title;
            fs.innerText = sub;
            ft.style.color = (type == 'goal-blue') ? 'var(--jedi)' : (type == 'goal-red' ? 'var(--sith)' : 'var(--gold)');
            ft.style.textShadow = '0 0 50px ' + ft.style.color;
            
            f.style.display = 'flex';
            setTimeout(() => f.classList.add('flash-active'), 50);
            setTimeout(() => {
                f.classList.remove('flash-active');
                setTimeout(() => f.style.display = 'none', 300);
            }, 3000);
        }

        function connectWS() {
            let ws = new WebSocket('ws://' + location.hostname + ':81/');
            ws.onmessage = function(event) {
                let data = JSON.parse(event.data);
                const t1 = document.getElementById('t1_name').innerText;
                const t2 = document.getElementById('t2_name').innerText;
                let raw_m = data.m;
                let display_m = "";
                
                const random = (arr) => arr[Math.floor(Math.random() * arr.length)];

                if(raw_m == "B1") {
                    const phrases = [
                        `🔵 [BUT ${t1}] La Force est avec vous !`,
                        `🔵 [BUT ${t1}] Tir de précision d'un X-Wing !`,
                        `🔵 [BUT ${t1}] ${t1} transperce la défense adverse !`,
                        `🔵 [BUT ${t1}] Un coup de sabre laser fatal !`
                    ];
                    showFlash('goal-blue', 'BUT !', t1 + ' MARQUE');
                    display_m = `<span style="color:var(--jedi)">${random(phrases)}</span>`;
                }
                else if(raw_m == "B2") {
                    const phrases = [
                        `🔴 [BUT ${t2}] ${t2} contre-attaque avec fureur !`,
                        `🔴 [BUT ${t2}] Le côté obscur marque un point !`,
                        `🔴 [BUT ${t2}] La vengeance de ${t2} est en marche !`,
                        `🔴 [BUT ${t2}] Tir dévastateur !`
                    ];
                    showFlash('goal-red', 'BUT !', t2 + ' MARQUE');
                    display_m = `<span style="color:var(--sith)">${random(phrases)}</span>`;
                }
                else if(raw_m == "G1") {
                    const phrases = [
                        `💎 [GAMELLE ${t1}] Explosion de l'Étoile Noire !`,
                        `💎 [GAMELLE ${t1}] Maître Yoda valide ce tir légendaire !`,
                        `💎 [GAMELLE ${t1}] La Force est surpuissante !`
                    ];
                    showFlash('goal-blue', 'GAMELLE !', t1 + ' EST SURPUISSANT');
                    display_m = `<span style="color:var(--jedi)">${random(phrases)}</span>`;
                }
                else if(raw_m == "G2") {
                    const phrases = [
                        `🔥 [GAMELLE ${t2}] L'Empereur jubile !`,
                        `🔥 [GAMELLE ${t2}] Une perturbation majeure !`,
                        `🔥 [GAMELLE ${t2}] Le côté obscur triomphe !`
                    ];
                    showFlash('goal-red', 'GAMELLE !', t2 + ' EST SURPUISSANT');
                    display_m = `<span style="color:var(--sith)">${random(phrases)}</span>`;
                }
                else if(raw_m == "demi_j1" || raw_m == "demi_j2") {
                    const phrases = [
                        `✨ [DEMI] Une perturbation dans la Force... Point annulé !`,
                        `✨ [DEMI] Saut dans l'hyper-espace ! On revient en arrière.`,
                        `✨ [DEMI] Manipulation mentale... Le point disparaît !`
                    ];
                    showFlash('goal-gold', 'DEMI !', 'PERTURBATION DANS LA FORCE');
                    display_m = `<span style="color:var(--gold)">${random(phrases)}</span>`;
                }
                else if(raw_m.includes("lance")) display_m = `⚔️ <b>LE DUEL COMMENCE</b> : ${t1} vs ${t2}`;
                else if(raw_m == "victoire_p1") display_m = `🏆 VICTOIRE DE ${t1} ! La Galaxie est sauvée.`;
                else if(raw_m == "victoire_p2") display_m = `🏆 VICTOIRE DE ${t2} ! Le côté obscur triomphe.`;
                else display_m = raw_m;
                
                if (display_m) {
                    let logBox = document.getElementById('log-box');
                    logBox.innerHTML = `<div class="log-entry">${display_m}</div>` + logBox.innerHTML;
                }
            };
            ws.onclose = () => setTimeout(connectWS, 2000);
        }
        connectWS();

        function loadLogs() {
            fetch('/api/tv_events').then(r => r.json()).then(data => {
                if(data.logs) {
                    let html = '';
                    data.logs.forEach(l => {
                        let m = l.m;
                        if(m == "B1") m = "🔵 BUT JEDI";
                        if(m == "B2") m = "🔴 BUT SITH";
                        if(m == "G1") m = "💎 GAMELLE JEDI";
                        if(m == "G2") m = "🔥 GAMELLE SITH";
                        if(m == "demi_j1" || m == "demi_j2") m = "✨ DEMI";
                        html += `<div class="log-entry">${m}</div>`;
                    });
                    document.getElementById('log-box').innerHTML = html;
                }
            });
        }
        loadLogs();



        function update() {
            fetch('/api/status').then(r => r.json()).then(data => {
                document.getElementById('s1').innerText = data.p1;
                document.getElementById('s2').innerText = data.p2;
                document.getElementById('ball').innerText = data.ball;
                document.getElementById('t1_name').innerText = data.t1;
                document.getElementById('t2_name').innerText = data.t2;
                
                // Gestion de l'indicateur DEMI global
                document.getElementById('demi-global').style.display = (data.waiting > 0) ? 'block' : 'none';

                let ov = document.getElementById('overlay');


                let label = document.getElementById('match-label');

                if(data.finished) {
                    label.innerText = "VICTOIRE !"; label.style.color = "#0f0";
                    let winner = (data.p1 > data.p2) ? data.t1 : data.t2;
                    ov.innerHTML = `<h1>🏆 VAINQUEUR 🏆</h1><div class="winner-name">${winner}</div><div class="quote">La Force est puissante en vous. Le duel est terminé.</div>`;
                    ov.style.display = "flex";
                } else if(!data.run) {
                    label.innerText = "EN ATTENTE"; label.style.color = "#888";
                    ov.innerHTML = `<h1>PROCHAIN MATCH</h1><div class="winner-name" style="font-size:5vw">${data.t1} vs ${data.t2}</div><div class="quote">Préparez vos sabres laser... La galaxie attend son champion.</div>`;
                    ov.style.display = "flex";
                } else {
                    label.innerText = "MATCH EN COURS"; label.style.color = "var(--gold)";
                    ov.style.display = "none";
                }

                if(data.run && !isRunning) { startTime = Date.now(); isRunning = true; }
                if(!data.run) isRunning = false;
                if(isRunning) {
                    let diff = Math.floor((Date.now() - startTime) / 1000);
                    let m = Math.floor(diff/60).toString().padStart(2,'0');
                    let s = (diff%60).toString().padStart(2,'0');
                    document.getElementById('timer').innerText = m + ":" + s;
                }
            });
        }

        function updateTournament() {
            fetch('/api/get_tournament').then(r => r.json()).then(data => {
                tournament = data;
                document.getElementById('bracket-box').innerHTML = data.rounds.map((round, rIdx) => `
                    <div class="round">
                        ${round.map((m, mIdx) => `
                            <div class="match ${tournament.activeMatch?.r==rIdx && tournament.activeMatch?.m==mIdx ? 'active' : ''}">
                                <div class="team ${m.winner==1?'winner':''}"><span>${m.t1 || '?'}</span><span class="score">${m.s1}</span></div>
                                <div class="team ${m.winner==2?'winner':''}"><span>${m.t2 || '?'}</span><span class="score">${m.s2}</span></div>
                            </div>
                        `).join('')}
                    </div>
                `).join('');
            });
        }

        function save() {
            if(!tournament.teams || tournament.teams.length == 0) return Promise.resolve();
            return fetch('/api/set_tournament', { 
                method: 'POST', 
                headers: {'Content-Type': 'text/plain'},
                body: JSON.stringify(tournament) 
            });
        }
        setInterval(update, 3000); 
        setInterval(updateTournament, 60000); // Mise à jour lente du tableau (60s)
        update(); updateTournament();
    </script>
    <a href="/" style="position:fixed; bottom:10px; right:10px; color:rgba(255,255,255,0.2); text-decoration:none; font-size:10px;">CONSOLE</a>
</body>
</html>
)rawliteral";

const char WIFI_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WiFi Setup - Star Wars Console</title>
    <style>
        :root { --gold: #ffe000; --bg: #0d1117; --card: #161b22; --blue: #58a6ff; }
        body { background: var(--bg); color: #c9d1d9; font-family: sans-serif; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }
        .card { background: var(--card); border: 1px solid #30363d; border-radius: 20px; padding: 30px; width: 90%; max-width: 400px; box-shadow: 0 10px 40px rgba(0,0,0,0.6); }
        h1 { color: var(--gold); font-size: 1.5rem; text-transform: uppercase; margin-top: 0; text-align: center; }
        .network-item { padding: 12px; border-bottom: 1px solid #30363d; cursor: pointer; transition: 0.2s; display: flex; justify-content: space-between; }
        .network-item:hover { background: rgba(255,255,0,0.1); }
        input { width: 100%; padding: 12px; margin: 15px 0; background: #0d1117; border: 1px solid #30363d; border-radius: 8px; color: white; box-sizing: border-box; }
        .btn { display: block; width: 100%; padding: 15px; background: var(--gold); border: none; border-radius: 10px; color: black; font-weight: bold; cursor: pointer; text-transform: uppercase; }
        .btn:disabled { background: #333; color: #888; }
        .scan-status { font-size: 12px; color: var(--blue); text-align: center; margin-bottom: 15px; }
        .loader { border: 3px solid #333; border-top: 3px solid var(--gold); border-radius: 50%; width: 20px; height: 20px; animation: spin 1s linear infinite; display: inline-block; margin-right: 10px; }
        @keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
    </style>
</head>
<body>
    <div class="card">
        <h1>CONFIGURATION WIFI</h1>
        <div id="scan-box">
            <div class="scan-status" id="status"><div class="loader"></div>Recherche des réseaux...</div>
            <div id="networks"></div>
        </div>
        <form id="wifi-form" style="display:none;" onsubmit="saveWiFi(event)">
            <p>Connecter à : <b id="selected-ssid"></b></p>
            <input type="hidden" id="ssid" name="ssid">
            <input type="password" id="pass" name="pass" placeholder="Mot de passe WiFi" required>
            <button type="submit" class="btn" id="save-btn">SE CONNECTER</button>
            <button type="button" class="btn" style="background:#333; color:#fff; margin-top:10px;" onclick="cancel()">RETOUR</button>
        </form>
        <div id="success" style="display:none; text-align:center;">
            <p style="color:var(--gold); font-weight:bold;">Identifiants enregistrés !</p>
            <p>L'ESP32 va redémarrer et tenter de se connecter. Cherchez-le sur votre réseau local (babyfoot.local).</p>
        </div>
        <br>
        <br>
        <a href="/update" style="color:var(--gold); display:block; text-align:center; text-decoration:none; font-size:12px; margin-top:5px;">MISE À JOUR OTA</a>
        <a href="/" style="color:#8b949e; display:block; text-align:center; text-decoration:none; font-size:12px;">RETOUR CONSOLE</a>
    </div>

    <script>
        function scan() {
            fetch('/api/wifi_scan').then(r => r.json()).then(data => {
                let html = '';
                data.forEach(net => {
                    html += `<div class="network-item" onclick="select('${net.ssid}')">
                        <span>${net.ssid}</span>
                        <span style="color:var(--gold)">${net.rssi}dBm</span>
                    </div>`;
                });
                document.getElementById('networks').innerHTML = html;
                document.getElementById('status').innerText = data.length + " réseaux trouvés";
            });
        }

        function select(ssid) {
            document.getElementById('scan-box').style.display = 'none';
            document.getElementById('wifi-form').style.display = 'block';
            document.getElementById('selected-ssid').innerText = ssid;
            document.getElementById('ssid').value = ssid;
            document.getElementById('pass').focus();
        }

        function cancel() {
            document.getElementById('scan-box').style.display = 'block';
            document.getElementById('wifi-form').style.display = 'none';
        }

        function saveWiFi(e) {
            e.preventDefault();
            const ssid = document.getElementById('ssid').value;
            const pass = document.getElementById('pass').value;
            document.getElementById('save-btn').disabled = true;
            document.getElementById('save-btn').innerText = "ENREGISTREMENT...";

            fetch('/api/save_wifi', {
                method: 'POST',
                headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                body: 'ssid=' + encodeURIComponent(ssid) + '&pass=' + encodeURIComponent(pass)
            }).then(r => {
                    document.getElementById('wifi-form').style.display = 'none';
                    document.getElementById('success').style.display = 'block';
                });
        }

        scan();
        setInterval(scan, 10000);
    </script>
</body>
</html>
)rawliteral";

const char UPDATE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8"><title>Mise à jour Force</title>
    <style>
        :root { --gold: #ffe000; --bg: #0d1117; }
        body { background: var(--bg); color: #c9d1d9; font-family: sans-serif; text-align: center; padding: 50px; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 80vh; }
        h1 { color: var(--gold); }
        .card { background: #161b22; padding: 30px; border-radius: 15px; border: 1px solid #30363d; }
        .btn { display: inline-block; padding: 15px 30px; background: #21262d; border: 1px solid #30363d; border-radius: 10px; color: white; text-decoration: none; font-weight: bold; margin: 10px; cursor: pointer; }
        .btn-gold { border-color: var(--gold); color: var(--gold); }
        #prg { margin-top: 20px; color: var(--gold); display: none; }
    </style>
</head>
<body>
    <div class="card">
        <h1>MISE À JOUR V2.7</h1>
        <p>Sélectionnez le fichier <b>.bin</b> compilé.</p>
        <form method='POST' action='/do_update' enctype='multipart/form-data' id='upload_form'>
            <input type='file' name='update' style="margin-bottom:20px;"><br>
            <input type='submit' value='LANCER LA MISE À JOUR' class="btn btn-gold" onclick="document.getElementById('prg').style.display='block'">
        </form>
        <div id="prg">Transfert en cours... Ne pas éteindre le babyfoot.</div>
    </div>
    <br>
    <a href="/" class="btn">RETOUR CONSOLE</a>
</body>
</html>
)rawliteral";

const char TOURNAMENT_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8"><title>Tournoi Force - Bracket</title>
    <style>
        :root { --gold: #ffe000; --bg: #0b0e14; --card: #161b22; }
        body { background: var(--bg); color: white; font-family: 'Segoe UI', sans-serif; margin: 0; padding: 20px; text-align: center; }
        .bracket-container { display: flex; justify-content: center; gap: 50px; margin-top: 40px; overflow-x: auto; padding: 20px; }
        .round { display: flex; flex-direction: column; justify-content: space-around; gap: 20px; }
        .match { background: var(--card); border: 2px solid #333; border-radius: 10px; width: 200px; padding: 12px; position: relative; transition: 0.3s; cursor: pointer; }
        .match.active { border-color: var(--gold); box-shadow: 0 0 15px var(--gold); }
        .match.finished { border-color: #2ea043; opacity: 0.8; }
        .team { display: flex; justify-content: space-between; padding: 5px; font-weight: bold; border-radius: 5px; }
        .team.winner { color: var(--gold); }
        .score { color: #8b949e; }
        .btn { padding: 10px 20px; background: var(--gold); color: black; border: none; border-radius: 5px; font-weight: bold; cursor: pointer; margin: 10px; }
        .setup-panel { background: var(--card); padding: 20px; border-radius: 15px; max-width: 600px; margin: 0 auto 30px; border: 1px solid #333; }
        input { padding: 10px; background: #000; color: #fff; border: 1px solid #333; border-radius: 5px; width: 180px; }
        .next-btn { display:block; width:100%; margin-top:10px; padding:10px; background:#0f0; color:#000; border:none; border-radius:5px; font-weight:bold; cursor:pointer; animation: pulse 1.5s infinite; text-transform:uppercase; font-size:0.8rem; }
        @keyframes pulse { 0% { background:#0f0; box-shadow:0 0 0px #0f0; } 50% { background:#0a0; box-shadow:0 0 15px #0f0; } 100% { background:#0f0; box-shadow:0 0 0px #0f0; } }
    </style>
</head>
<body>
    <div style="display:flex; justify-content: space-between; align-items: center; max-width: 800px; margin: 0 auto; padding: 10px;">
        <button class="btn" style="background:#333; color:#fff" onclick="location.href='/'">RETOUR</button>
        <h1 style="margin:0">CHAMPIONNAT STAR WARS</h1>
        <div style="width:100px"></div>
    </div>
    <div id="setup" class="setup-panel">
        <h3>INSCRIPTION DES EQUIPES (4 a 8)</h3>
        <p style="color:#8b949e; font-size:0.9rem;">Ajoutez entre 4 et 8 equipes. Les places vides seront completees par des "BYES".</p>
        <input type="text" id="tname" placeholder="Nom de l'Equipe">
        <button class="btn" onclick="add()">AJOUTER</button>
        <div id="team_list" style="margin:15px; columns:2; text-align:left;"></div>
        <button class="btn" onclick="startTournament()" id="start_btn" disabled>GENERER LE TABLEAU</button>
    </div>
    <div style="margin-top:20px;"><button class="btn" style="background:#d73a49; color:white; width: 220px;" onclick="reset()">RAZ MEMOIRE & RESET</button></div>
    <div class="bracket-container" id="bracket"></div>
    <script>
        let tournament = { teams: [], rounds: [], activeMatch: null };
        function add() {
            let n = document.getElementById('tname').value.trim().toUpperCase();
            if(n && tournament.teams.length < 8) {
                tournament.teams.push(n); document.getElementById('tname').value = ''; render();
            }
        }
        function startTournament() {
            let teams = [...tournament.teams];
            let bracketSize = (teams.length <= 4) ? 4 : 8;
            while(teams.length < bracketSize) teams.push("BYE");
            let shuffled = teams.sort(() => Math.random() - 0.5);
            tournament.rounds = [[]];
            for(let i=0; i<shuffled.length; i+=2) {
                let m = { t1: shuffled[i], t2: shuffled[i+1], s1:0, s2:0, winner:null };
                if(m.t1 == "BYE" && m.t2 == "BYE") { m.winner = null; }
                else if(m.t2 == "BYE") { m.winner = 1; }
                else if(m.t1 == "BYE") { m.winner = 2; }
                tournament.rounds[0].push(m);
            }
            let nextSize = bracketSize / 4;
            while(nextSize >= 1) {
                let r = []; for(let i=0; i<nextSize; i++) r.push({ t1: null, t2: null, s1:0, s2:0, winner:null });
                tournament.rounds.push(r); nextSize /= 2;
            }
            tournament.rounds[0].forEach((m, idx) => {
                if(m.winner) {
                    let winName = (m.winner == 1) ? m.t1 : m.t2;
                    let nextM = Math.floor(idx / 2);
                    if(idx % 2 == 0) tournament.rounds[1][nextM].t1 = winName;
                    else tournament.rounds[1][nextM].t2 = winName;
                }
            });
            save(); render();
        }
        function nextMatchAuto() {
            for(let r=0; r<tournament.rounds.length; r++) {
                for(let mIdx=0; mIdx<tournament.rounds[r].length; mIdx++) {
                    let m = tournament.rounds[r][mIdx];
                    if(!m.winner && m.t1 && m.t2 && m.t1 != "BYE" && m.t2 != "BYE") {
                        launch(r, mIdx); return;
                    }
                }
            }
            alert("Tournoi termine ! Plus de match a lancer.");
        }
        function launch(r, m) {
            let match = tournament.rounds[r][m];
            if(!match.t1 || !match.t2 || match.t1 == "BYE" || match.t2 == "BYE" || match.winner) return;
            tournament.activeMatch = { r, m };
            save().then(() => {
                fetch(`/api/save_settings?t1=${match.t1}&t2=${match.t2}&mode=1`).then(() => {
                    alert("Match lance : " + match.t1 + " vs " + match.t2); render();
                });
            });
        }
        function render() {
            if(tournament.rounds.length == 0) {
                document.getElementById('setup').style.display = 'block';
                document.getElementById('team_list').innerHTML = tournament.teams.map(t => `<div>- ${t}</div>`).join('');
                document.getElementById('start_btn').disabled = (tournament.teams.length < 4);
                document.getElementById('bracket').innerHTML = ''; return;
            }
            document.getElementById('setup').style.display = 'none';
            document.getElementById('bracket').innerHTML = tournament.rounds.map((round, rIdx) => {
                if(round.length == 0) return '';
                return `
                <div class="round">
                    <div style="font-weight:bold; margin-bottom:10px; color:var(--gold)">
                        ${tournament.rounds.length==2 ? (rIdx==0?'SEMIS':rIdx==1?'FINALE':'VAINQUEUR') : (rIdx==0?'QUARTS':rIdx==1?'SEMIS':rIdx==2?'FINALE':'VAINQUEUR')}
                    </div>
                    ${round.map((m, mIdx) => `
                        <div class="match ${tournament.activeMatch?.r==rIdx && tournament.activeMatch?.m==mIdx ? 'active' : ''} ${m.winner ? 'finished' : ''}" 
                             onclick="launch(${rIdx},${mIdx})">
                            <div class="team ${m.winner==1?'winner':''}">
                                <span>${m.t1 || '?'}</span>
                                <span class="score">${(m.t1=="BYE" || m.t2=="BYE") && m.winner ? '-' : m.s1}</span>
                            </div>
                            <div class="team ${m.winner==2?'winner':''}">
                                <span>${m.t2 || '?'}</span>
                                <span class="score">${(m.t1=="BYE" || m.t2=="BYE") && m.winner ? '-' : m.s2}</span>
                            </div>
                        </div>
                    `).join('')}
                </div>
                `;
            }).join('');
        }
        function toast(msg) {
            let t = document.createElement('div');
            t.style = "position:fixed; top:20px; left:50%; transform:translateX(-50%); background:rgba(0,255,100,0.9); color:black; padding:15px 30px; border-radius:10px; font-weight:bold; z-index:10000; box-shadow:0 0 20px rgba(0,255,100,0.5); border:2px solid #fff; font-family:sans-serif;";
            t.innerText = msg;
            document.body.appendChild(t);
            setTimeout(() => t.remove(), 5000);
        }

        function syncResults(status) {
            if(!status) return;
            if(status.finished && tournament.activeMatch) {
                tournament.activeMatch = null;
                toast("MATCH TERMINE ! (Mise a jour automatique)");
                load();
            }
        }
        function reset() { 
            if(confirm("REINITIALISATION TOTALE (Tournoi + Scores) ?")) { 
                fetch('/api/reset_tournament').then(() => {
                    alert("Mémoire effacée. Redémarrage...");
                    location.href = '/';
                });
            } 
        }
        function save() { 
            return fetch('/api/set_tournament', { 
                method: 'POST', 
                headers: {'Content-Type': 'text/plain'},
                body: JSON.stringify(tournament) 
            });
        }
        function load() { 
            fetch('/api/get_tournament').then(r => r.json()).then(data => { 
                tournament = data; render(); 
            }).catch(e => console.log("Pas de tournoi actif.")); 
        }
        
        function updateStatus() {
            fetch('/api/status').then(r => r.json()).then(data => {
                if(typeof syncResults === 'function') syncResults(data); 
            });
        }
        setInterval(updateStatus, 5000); load(); // Polling réduit pour la console de gestion
    </script>
</body>
</html>
)rawliteral";

const char LOGS_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8"><title>System Logs</title>
    <style>
        body { background: #000; color: #0f0; font-family: monospace; padding: 20px; }
        .log-container { background: #111; border: 1px solid #333; padding: 15px; height: 80vh; overflow-y: auto; display: flex; flex-direction: column-reverse; }
        .log-entry { border-bottom: 1px solid #222; padding: 5px 0; }
        .time { color: #888; margin-right: 10px; }
        .btn { padding: 10px; background: #333; color: #fff; border: none; cursor: pointer; margin-bottom: 10px; }
    </style>
</head>
<body>
    <h1>CONSOLE DE TELEMETRIE</h1>
    <button class="btn" onclick="location.href='/'">RETOUR</button>
    <div class="log-container" id="log-box"></div>
    <script>
        function update() {
            fetch('/api/logs').then(r => r.json()).then(data => {
                document.getElementById('log-box').innerHTML = data.logs.map(l => `<div class="log-entry"><span class="time">[${l.t}]</span>${l.m}</div>`).join('');
            });
        }
        setInterval(update, 3000); update();
    </script>
</body>
</html>
)rawliteral";

const char AP_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Babyfoot Force - Setup</title>
    <style>
        :root { --gold: #ffe000; --bg: #0d1117; --card: rgba(22,27,34,0.9); --blue: #58a6ff; }
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { background: var(--bg); color: #c9d1d9; font-family: sans-serif; display: flex; justify-content: center; align-items: center; min-height: 100vh; }
        .container { width: 90%; max-width: 400px; }
        .logo { text-align: center; margin-bottom: 30px; }
        .logo h1 { color: var(--gold); font-size: 1.6rem; text-transform: uppercase; letter-spacing: 3px; text-shadow: 0 0 20px rgba(255,224,0,0.3); }
        .logo p { color: #8b949e; font-size: 0.85rem; margin-top: 5px; }
        .card { background: var(--card); border: 1px solid #30363d; border-radius: 20px; padding: 25px; box-shadow: 0 10px 40px rgba(0,0,0,0.5); }
        .btn { display: flex; align-items: center; gap: 15px; padding: 18px 20px; background: #21262d; border: 1px solid #30363d; border-radius: 12px; color: white; text-decoration: none; font-weight: bold; font-size: 1rem; margin-bottom: 12px; transition: all 0.2s; }
        .btn:last-child { margin-bottom: 0; }
        .btn:hover { background: #30363d; transform: translateY(-2px); box-shadow: 0 4px 15px rgba(0,0,0,0.3); }
        .btn:active { transform: scale(0.98); }
        .btn-wifi { border-color: var(--gold); }
        .btn-wifi:hover { box-shadow: 0 0 20px rgba(255,224,0,0.2); }
        .btn-icon { font-size: 1.5rem; width: 35px; text-align: center; }
        .btn-label { flex: 1; }
        .btn-label small { display: block; color: #8b949e; font-weight: normal; font-size: 0.75rem; margin-top: 2px; }
        .version { text-align: center; margin-top: 20px; color: #30363d; font-size: 0.7rem; }
        .pulse { animation: pulse 2s infinite; }
        @keyframes pulse { 0%,100% { opacity: 1; } 50% { opacity: 0.5; } }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">
            <h1>⚡ BABYFOOT FORCE</h1>
            <p class="pulse">MODE CONFIGURATION</p>
        </div>
        <div class="card">
            <a href="/wifi" class="btn btn-wifi">
                <span class="btn-icon">📡</span>
                <span class="btn-label">CONFIGURATION WIFI<small>Connecter a votre reseau</small></span>
            </a>
            <a href="/logs" class="btn">
                <span class="btn-icon">📋</span>
                <span class="btn-label">LOGS SYSTEME<small>Diagnostics et evenements</small></span>
            </a>
            <a href="/update" class="btn">
                <span class="btn-icon">🔄</span>
                <span class="btn-label">MISE A JOUR OTA<small>Flasher un nouveau firmware</small></span>
            </a>
        </div>
        <div class="version">BABYFOOT FORCE V2.8</div>
    </div>
</body>
</html>
)rawliteral";

#endif