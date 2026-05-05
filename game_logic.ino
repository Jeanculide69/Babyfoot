#include <Arduino.h>
#include "config.h"

// --- ETAT DU JEU ---
int score_rouge = 0;
int score_bleu = 0;
int balles_restantes = 11;
unsigned long match_start_millis = 0;
bool match_active = false;

// Flags de jeu
#define STATE_STANDBY   0
#define STATE_PICK_TEAM 1
#define STATE_MATCH     2
#define STATE_GAME_OVER 3
int current_game_state = STATE_STANDBY;

// Système d'événements et animations
String current_event_text = "BABYFOOT";
uint16_t current_event_color = 0xFFFF;
unsigned long event_display_end = 0;

// Externs Audio & Strip & Logs
extern void playSound(int track);
extern void playVictorySound();
extern void flashGoal(uint32_t color);
extern void setAmbilightMode(int mode);
extern void addLog(String msg);

void triggerEvent(String text, uint16_t color, int sound_track) {
  current_event_text = text;
  current_event_color = color;
  event_display_end = millis() + 4000;
  if (sound_track > 0) playSound(sound_track);
}

void resetMatch() {
  score_rouge = 0;
  score_bleu = 0;
  balles_restantes = 11;
  match_start_millis = millis();
  match_active = true;
  current_game_state = STATE_MATCH;
  setAmbilightMode(1);
  addLog("MATCH START");
  triggerEvent("KICK OFF!", 0x07E0, 1);
}

void raz_but() {
  unsigned long start = millis();
  while ((digitalRead(GOAL_RIGHT) || digitalRead(GOAL_LEFT) || digitalRead(GAMELLE_RIGHT) || digitalRead(GAMELLE_LEFT)) && (millis() - start < 1000)) {
    digitalWrite(RESET_PIN, LOW);
    delay(10);
    digitalWrite(RESET_PIN, HIGH);
    delay(10);
  }
}

void handleGameLogic() {
  static unsigned long last_input_check = 0;
  if (millis() - last_input_check < 20) return; 
  last_input_check = millis();

  bool ok_pressed = (digitalRead(BTN_OK) == LOW);
  bool less_pressed = (digitalRead(BTN_LESS) == LOW);
  bool more_pressed = (digitalRead(BTN_MORE) == LOW);

  if (current_game_state == STATE_STANDBY) {
    if (ok_pressed) {
      current_game_state = STATE_PICK_TEAM;
      triggerEvent("EQUIPES?", 0xFFFF, 2);
    }
    return;
  }

  if (current_game_state == STATE_PICK_TEAM) {
    if (less_pressed || more_pressed) {
      triggerEvent("SWITCH!", 0x07FF, 0);
    }
    if (ok_pressed) resetMatch();
    return;
  }

  if (current_game_state == STATE_MATCH) {
    // --- NOUVEAU : Combo PLUS + MOINS (Mode Demi / Bière) ---
    if (less_pressed && more_pressed) {
      addLog("MODE DEMI ACTIVE");
      triggerEvent("DEMI ! BERE?", 0xFFE0, 0); // Jaune
      delay(500);
    }

    // But Rouge (Droite)
    if (digitalRead(GOAL_RIGHT) == HIGH) {
      addLog("BUT ROUGE !");
      score_rouge++;
      balles_restantes--;
      flashGoal(0xF800);
      triggerEvent("BUT ROUGE!", 0xF800, 3);
      raz_but();
    }
    
    // But Bleu (Gauche)
    if (digitalRead(GOAL_LEFT) == HIGH) {
      addLog("BUT BLEU !");
      score_bleu++;
      balles_restantes--;
      flashGoal(0x001F);
      triggerEvent("BUT BLEU!", 0x001F, 3);
      raz_but();
    }

    // Gamelle Rouge (Droite)
    if (digitalRead(GAMELLE_RIGHT) == HIGH) {
      addLog("GAMELLE ROUGE !");
      score_bleu--; 
      triggerEvent("GAMELLE!", 0xF800, 4);
      raz_but();
    }
    
    // Gamelle Bleue (Gauche)
    if (digitalRead(GAMELLE_LEFT) == HIGH) {
      addLog("GAMELLE BLEUE !");
      score_rouge--; 
      triggerEvent("GAMELLE!", 0x001F, 4);
      raz_but();
    }

    if (balles_restantes <= 0) {
      current_game_state = STATE_GAME_OVER;
      match_active = false;
      playVictorySound();
      if (score_rouge > score_bleu) triggerEvent("VICTOIRE ROUGE", 0xF800, 0);
      else triggerEvent("VICTOIRE BLEUE", 0x001F, 0);
    }
  }

  if (current_game_state == STATE_GAME_OVER) {
    if (ok_pressed) current_game_state = STATE_STANDBY;
  }
}

String getMatchTime() {
  if (!match_active) return "00:00";
  unsigned long duration = (millis() - match_start_millis) / 1000;
  int minutes = duration / 60;
  int seconds = duration % 60;
  char buf[10];
  sprintf(buf, "%02d:%02d", minutes, seconds);
  return String(buf);
}

void handleAction(String a) {
  if (a == "r1") score_rouge++;
  else if (a == "b1") score_bleu++;
  else if (a == "rs") resetMatch();
}
