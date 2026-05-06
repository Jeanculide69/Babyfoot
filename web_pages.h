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
        .btn { display: block; padding: 12px; background: #21262d; border: 1px solid #30363d; border-radius: 10px; color: white; text-decoration: none; font-weight: bold; text-align: center; cursor: pointer; margin: 5px 0; }
        .btn:active { transform: scale(0.95); }
        .btn-jedi { border-color: var(--jedi); color: var(--jedi); }
        .btn-sith { border-color: var(--sith); color: var(--sith); }
        .btn-gold { border-color: var(--gold); color: var(--gold); }
        .nav-grid { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 10px; margin-top: 20px; }
        .action-row { display: flex; gap: 10px; margin-top: 10px; }
        .action-btn { flex: 1; font-size: 14px; border-width: 2px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="card">
            <h2>LIVE TELEMETRY</h2>
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
            <div style="text-align:center; margin-top:15px; color:var(--gold)">BALLES: <span id="ball">11</span></div>
        </div>

        <div class="card">
            <h2>COMMAND CENTER</h2>
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
            <div class="btn btn-gold" style="margin-top:15px;" onclick="doAct('OK')">START / RESET / OK</div>
        </div>

        <div class="nav-grid">
            <a href="/tournament" class="btn">TOURNOI</a>
            <a href="/tv" class="btn">TV LIVE</a>
            <a href="/logs" class="btn">LOGS</a>
            <a href="/update" class="btn" style="color:#8b949e">UPDATE</a>
        </div>
    </div>
    <script>
        function doAct(id){ fetch('/action?id='+id); }
        function update() {
            fetch('/api/status').then(r => r.json()).then(data => {
                document.getElementById('s1').innerText = data.p1;
                document.getElementById('s2').innerText = data.p2;
                document.getElementById('ball').innerText = data.ball;
                document.getElementById('t1_name').innerText = data.t1;
                document.getElementById('t2_name').innerText = data.t2;
            });
        }
        setInterval(update, 1000); update();
    </script>
</body>
</html>
)rawliteral";

const char TV_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8"><title>TV Dashboard</title>
    <style>
        body { background: #05070a; color: white; font-family: sans-serif; margin: 0; overflow: hidden; display: flex; flex-direction: column; height: 100vh; }
        .header { padding: 20px; text-align: center; border-bottom: 2px solid #ffe000; box-shadow: 0 0 20px #ffe000; }
        .main-score { flex: 1; display: flex; align-items: center; justify-content: space-around; padding: 20px; }
        .score-val { font-size: 18vw; font-weight: 900; text-shadow: 0 0 60px currentColor; border: 10px solid currentColor; padding: 20px 50px; border-radius: 40px; }
        .jedi { color: #00f2ff; } .sith { color: #ff0000; }
        .ball { position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); background:#111; border:4px solid #ffe000; padding:20px; border-radius:50%; font-size: 4vw; color: #ffe000; }
    </style>
</head>
<body>
    <div class="header"><h1>BABYFOOT FORCE - LIVE TELEMETRY</h1></div>
    <div class="main-score">
        <div class="jedi" style="text-align:center"><div id="t1_name" style="font-size:4vw;margin-bottom:20px">JEDI</div><div id="s1" class="score-val">0</div></div>
        <div class="ball"><div id="ball">11</div></div>
        <div class="sith" style="text-align:center"><div id="t2_name" style="font-size:4vw;margin-bottom:20px">SITH</div><div id="s2" class="score-val">0</div></div>
    </div>
    <script>
        setInterval(() => {
            fetch('/api/status').then(r => r.json()).then(data => {
                document.getElementById('s1').innerText = data.p1;
                document.getElementById('s2').innerText = data.p2;
                document.getElementById('ball').innerText = data.ball;
                document.getElementById('t1_name').innerText = data.t1;
                document.getElementById('t2_name').innerText = data.t2;
            });
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

const char TOURNAMENT_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8"><title>Gestion Tournoi</title>
    <style>
        body { background: #0b0e14; color: #ffe000; font-family: sans-serif; padding: 20px; text-align: center; }
        .card { background: #161b22; border-radius: 15px; padding: 20px; margin-bottom: 20px; border: 1px solid #333; }
        .btn { padding: 12px 25px; background: #ffe000; color: black; border: none; border-radius: 8px; cursor: pointer; font-weight: bold; margin: 5px; }
        input { padding: 12px; background: #000; color: #fff; border: 1px solid #ffe000; border-radius: 8px; width: 60%; }
        .match { background: #1c2128; border: 2px solid #ffe000; padding: 15px; margin: 10px; border-radius: 10px; display: inline-block; min-width: 150px; cursor: pointer; }
        .match:hover { background: #2d333b; }
    </style>
</head>
<body>
    <h1>CHAMPIONNAT FORCE 🏆</h1>
    <div class="card">
        <h3>INSCRIPTION DES ÉQUIPES</h3>
        <input type="text" id="tname" placeholder="Nom (4-8 lettres)">
        <button class="btn" onclick="add()">AJOUTER</button>
        <div id="list" style="margin-top:15px; text-align:left; columns: 2;"></div>
        <button class="btn" style="background:red;color:white" onclick="reset()">RESET TOUT</button>
    </div>
    <div class="card">
        <h3>ORGANIGRAMME DES MATCHS</h3>
        <div id="bracket"></div>
    </div>
    <script>
        let teams = [];
        function add() { 
            let n = document.getElementById('tname').value.toUpperCase();
            if(n && teams.length < 8) { teams.push(n); document.getElementById('tname').value=''; render(); }
        }
        function launch(t1, t2) {
            fetch(`/api/save_settings?t1=${t1}&t2=${t2}&mode=1`).then(() => alert("Match lancé sur l'écran LED !"));
        }
        function render() {
            document.getElementById('list').innerHTML = teams.map(t => `<div>• ${t}</div>`).join('');
            let b = document.getElementById('bracket'); b.innerHTML = "";
            for(let i=0; i<teams.length; i+=2) {
                if(i+1 < teams.length) {
                    b.innerHTML += `<div class="match" onclick="launch('${teams[i]}','${teams[i+1]}')">${teams[i]}<br>vs<br>${teams[i+1]}</div>`;
                }
            }
        }
        function reset() { if(confirm("Effacer le tournoi ?")) { teams = []; render(); } }
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
    <h1>CONSOLE DE TÉLÉMÉTRIE</h1>
    <button class="btn" onclick="location.href='/'">RETOUR</button>
    <div class="log-container" id="log-box"></div>
    <script>
        function update() {
            fetch('/api/logs').then(r => r.json()).then(data => {
                document.getElementById('log-box').innerHTML = data.logs.map(l => `<div class="log-entry"><span class="time">[${l.t}]</span>${l.m}</div>`).join('');
            });
        }
        setInterval(update, 2000); update();
    </script>
</body>
</html>
)rawliteral";

#endif
