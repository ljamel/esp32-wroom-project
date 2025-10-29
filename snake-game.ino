/*
  esp32_snake_game.ino
  Jeu Snake hébergé par l'ESP32 WROOM — aucun matériel additionnel requis.
  Connecte-toi au WiFi "ESP32_Game" (mot de passe "playesp32") et ouvre http://192.168.4.1
*/

#include <WiFi.h>
#include <WebServer.h>

// AP settings
const char* AP_SSID = "ESP32_Game";
const char* AP_PASS = "playesp32";

WebServer server(80);

// HTML/JS/CSS page (kept in PROGMEM)
const char index_html[] PROGMEM = R"rawliteral(
<!doctype html>
<html>
<head>
<meta charset="utf-8">
<title>ESP32 Snake</title>
<meta name="viewport" content="width=device-width,initial-scale=1"/>
<style>
  body{margin:0;background:#000;color:#0f0;font-family:monospace;display:flex;flex-direction:column;align-items:center;justify-content:flex-start;height:100vh}
  h1{margin:10px;color:#7ff}
  canvas{background:#071013;border:4px solid #0f0;border-radius:8px;touch-action:none}
  #controls{margin-top:8px}
  button{margin:4px;padding:8px 12px;border-radius:6px;border:0;background:#0f0;color:#000;font-weight:bold}
  #info{margin-top:6px;color:#9f9}
</style>
</head>
<body>
  <h1>ESP32 Snake</h1>
  <canvas id="c" width="360" height="360"></canvas>
  <div id="controls">
    <button onclick="press('up')">▲</button><button onclick="press('left')">◄</button>
    <button onclick="press('down')">▼</button><button onclick="press('right')">►</button>
    <button onclick="startGame()">Start</button>
    <button onclick="resetHS()">Reset HS</button>
  </div>
  <div id="info">Score: <span id="score">0</span> &nbsp; Highscore: <span id="hs">0</span></div>

<script>
(() => {
  const canvas = document.getElementById('c');
  const ctx = canvas.getContext('2d');
  const size = 18;                 // number of cells per row/col
  const cell = canvas.width / size;
  let snake = [{x:8,y:9},{x:7,y:9}];
  let dir = {x:1,y:0};
  let food = {};
  let running = false;
  let tick = null;
  let score = 0;
  const speed = 120; // ms per step

  // Highscore in localStorage
  let hs = parseInt(localStorage.getItem('esp32_snake_hs')||'0');
  document.getElementById('hs').innerText = hs;

  function rndCell(){ return {x:Math.floor(Math.random()*size), y:Math.floor(Math.random()*size)}; }
  function placeFood(){
    let tries=0;
    do { food = rndCell(); tries++; if(tries>200) break; } while(snake.some(s=>s.x===food.x && s.y===food.y));
  }

  function draw(){
    ctx.fillStyle = '#071013';
    ctx.fillRect(0,0,canvas.width,canvas.height);

    // grid faint
    ctx.strokeStyle = 'rgba(0,255,0,0.04)';
    ctx.lineWidth=1;
    for(let i=0;i<=size;i++){
      ctx.beginPath(); ctx.moveTo(i*cell,0); ctx.lineTo(i*cell,canvas.height); ctx.stroke();
      ctx.beginPath(); ctx.moveTo(0,i*cell); ctx.lineTo(canvas.width,i*cell); ctx.stroke();
    }

    // food
    ctx.fillStyle = '#ff4';
    roundRect(ctx, food.x*cell+2, food.y*cell+2, cell-4, cell-4, 4, true, false);

    // snake
    for(let i=0;i<snake.length;i++){
      const s=snake[i];
      ctx.fillStyle = i===0 ? '#0ff' : '#0f6';
      roundRect(ctx, s.x*cell+1, s.y*cell+1, cell-2, cell-2, 4, true, false);
    }
  }

  function roundRect(ctx,x,y,w,h,r,fill,stroke){
    if (typeof r === 'undefined') r = 5;
    ctx.beginPath();
    ctx.moveTo(x + r, y);
    ctx.arcTo(x + w, y,     x + w, y + h, r);
    ctx.arcTo(x + w, y + h, x,     y + h, r);
    ctx.arcTo(x,     y + h, x,     y,     r);
    ctx.arcTo(x,     y,     x + w, y,     r);
    ctx.closePath();
    if(fill){ ctx.fill(); }
    if(stroke){ ctx.stroke(); }
  }

  function step(){
    // compute new head
    const head = {x:(snake[0].x + dir.x + size)%size, y:(snake[0].y+dir.y+size)%size};
    // collision with self?
    if(snake.some((p,i)=>i>0 && p.x===head.x && p.y===head.y)){
      gameOver(); return;
    }
    snake.unshift(head);
    // eat?
    if(head.x===food.x && head.y===food.y){
      score++;
      document.getElementById('score').innerText = score;
      placeFood();
    } else {
      snake.pop();
    }
    draw();
  }

  function gameOver(){
    running=false;
    clearInterval(tick);
    tick = null;
    // update HS
    if(score>hs){
      hs=score; localStorage.setItem('esp32_snake_hs', hs);
      document.getElementById('hs').innerText = hs;
      alert('Game over — new highscore: '+score);
    } else {
      alert('Game over — score: '+score);
    }
  }

  function startGame(){
    snake = [{x:8,y:9},{x:7,y:9}];
    dir = {x:1,y:0};
    score = 0; document.getElementById('score').innerText = score;
    placeFood();
    draw();
    if(tick) clearInterval(tick);
    running=true;
    tick = setInterval(step, speed);
  }

  // controls
  let lastPress = 0;
  window.addEventListener('keydown', e=>{
    if(Date.now()-lastPress<50) return;
    lastPress = Date.now();
    if(e.key==='ArrowUp' || e.key==='w') setDir(0,-1);
    if(e.key==='ArrowDown' || e.key==='s') setDir(0,1);
    if(e.key==='ArrowLeft' || e.key==='a') setDir(-1,0);
    if(e.key==='ArrowRight' || e.key==='d') setDir(1,0);
    if(e.key===' ') { if(!running) startGame(); }
  });

  function setDir(x,y){
    // prevent reverse
    if(dir.x === -x && dir.y === -y) return;
    dir = {x:x,y:y};
  }

  // touch controls for mobile (swipe)
  let touchStart = null;
  canvas.addEventListener('touchstart', e => { touchStart = e.touches[0]; e.preventDefault(); });
  canvas.addEventListener('touchmove', e => { e.preventDefault(); });
  canvas.addEventListener('touchend', e => {
    if(!touchStart) return;
    let t = e.changedTouches[0];
    let dx = t.clientX - touchStart.clientX;
    let dy = t.clientY - touchStart.clientY;
    if(Math.abs(dx)>Math.abs(dy)){
      if(dx>20) setDir(1,0); else if(dx<-20) setDir(-1,0);
    } else {
      if(dy>20) setDir(0,1); else if(dy<-20) setDir(0,-1);
    }
    touchStart=null;
  });

  // expose small API functions
  window.press = function(k){
    if(k==='up') setDir(0,-1);
    if(k==='down') setDir(0,1);
    if(k==='left') setDir(-1,0);
    if(k==='right') setDir(1,0);
  };
  window.startGame = startGame;
  window.resetHS = function(){ hs=0; localStorage.setItem('esp32_snake_hs',0); document.getElementById('hs').innerText = 0; };

  // initial draw and food
  placeFood();
  draw();

})();
</script>
</body>
</html>
)rawliteral";

void handleRoot(){
  server.send_P(200, "text/html", index_html);
}

void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress IP = WiFi.softAPIP();
  Serial.println();
  Serial.print("ESP32 Game AP '"); Serial.print(AP_SSID); Serial.print("' IP: "); Serial.println(IP);

  server.on("/", handleRoot);
  server.begin();
}

void loop(){
  server.handleClient();
}
