
#include "animations/But_J1.c"
#include "animations/But_J2.c"
#include "animations/Gamelle_J1.c"
#include "animations/Gamelle_J2.c"
#include "animations/Intro.c"
#include "animations/open.c"
#include "animations/soccer.c"


/*
 * Animation But_J1
 */

int read_but_j1(unsigned long speed_frame){
  
/* Read matrix picture */

if (frame== 0 or (last_frame + speed_frame)< millis()){   
  for(int y=0; y<32; y++){ 
  
              for (int x=0; x<64; x++){
                int pix= y*64+x;
                unsigned int color= but_j1_data[frame][pix];                    // Mettre à jour le nom en fonction de l'animation
                //Serial.println(color,HEX);
                matrix.drawPixel(x, y, convert12to16(color));
                uint32_t color_32 = convert_16bit_color(color);  
                edge_color(x,y,color_32);                                     // anime les rubans LED
                 
                }
   }
   last_frame=millis();
   frame++;
   strip1.show();
   strip2.show();
}
if (frame>=BUT_J1_FRAME_COUNT){if(!bitRead(statut_animation,LOOP)){bitSet(statut_animation,FINISH);}frame=0;}  // Mettre à jour le nom du FRAME_COUNT

}


/*
 * Animation But J2
 */

int read_but_j2(unsigned long speed_frame){
  
/* Read matrix picture */

if (frame == 0 or (last_frame + speed_frame)< millis()){   
  for(int y=0; y<32; y++){ 
  
              for (int x=0; x<64; x++){
                int pix= y*64+x;
                unsigned int color= but_j2_data[frame][pix];
                //Serial.println(color,HEX);
                matrix.drawPixel(x, y, convert12to16(color));
              
               uint32_t color_32 = convert_16bit_color(color);
               edge_color(x,y,color_32);                          // Anime les rubans Led

              }                                
   }
                 
   last_frame=millis();
   frame++;
   strip1.show();
   strip2.show();
}
if (frame>=BUT_J2_FRAME_COUNT){if(!bitRead(statut_animation,LOOP)){bitSet(statut_animation,FINISH);}frame=0;}
}

/*
 * Animation Gammelle J1
 */

int read_gamelle_j1(unsigned long speed_frame){
  
/* Read matrix picture */

if (frame == 0 or (last_frame + speed_frame)< millis()){   
  for(int y=0; y<32; y++){ 
  
              for (int x=0; x<64; x++){
                int pix= y*64+x;
                unsigned int color= gamelle_j1_data[frame][pix];      // !!! Changer le nom de la data par le nom qui ce trouve dans le fichier
                //Serial.println(color,HEX);
                matrix.drawPixel(x, y, convert12to16(color));
              
               uint32_t color_32 = convert_16bit_color(color);
               edge_color(x,y,color_32);                        // Anime les rubans led

              }                                
   }
                 
   last_frame=millis();
   frame++;
   strip1.show();
   strip2.show();
   
}
if (frame>=GAMELLE_J1_FRAME_COUNT){if(!bitRead(statut_animation,LOOP)){bitSet(statut_animation,FINISH);}frame=0;} // !!! Utiliser le nom du FRAME_COUNT
}

/*
 * Animation soccer
 */

int read_soccer(unsigned long speed_frame){
  
/* Read matrix picture */

if (frame == 0 or (last_frame + speed_frame)< millis()){   
  for(int y=0; y<32; y++){ 
  
              for (int x=0; x<64; x++){
                int pix= y*64+x;
                unsigned int color= soccer_data[frame][pix];      // !!! Changer le nom de la data par le nom qui ce trouve dans le fichier
                //Serial.println(color,HEX);
                matrix.drawPixel(x, y, convert12to16(color));
              
               uint32_t color_32 = convert_16bit_color(color);
               edge_color(x,y,color_32);                        // Anime les rubans led

              }                                
   }
                 
   last_frame=millis();
   frame++;
   strip1.show();
   strip2.show();
   
}
if (frame>=SOCCER_FRAME_COUNT){if(!bitRead(statut_animation,LOOP)){bitSet(statut_animation,FINISH);} else {frame=0;}} // !!! Utiliser le nom du FRAME_COUNT
}

/*
 * Animation ga
 */

int read_open(unsigned long speed_frame){
  
/* Read matrix picture */

if (frame == 0 or (last_frame + speed_frame)< millis()){   
  for(int y=0; y<32; y++){ 
  
              for (int x=0; x<64; x++){
                int pix= y*64+x;
                unsigned int color= open_data[frame][pix];      // !!! Changer le nom de la data par le nom qui ce trouve dans le fichier
                //Serial.println(color,HEX);
                matrix.drawPixel(x, y, convert12to16(color));
              
               uint32_t color_32 = convert_16bit_color(color);
               edge_color(x,y,color_32);                        // Anime les rubans led

              }                                
   }
                 
   last_frame=millis();
   frame++;
   strip1.show();
   strip2.show();
   
}
if (frame>=OPEN_FRAME_COUNT){if(!bitRead(statut_animation,LOOP)){bitSet(statut_animation,FINISH);} else {frame=0;}} // !!! Utiliser le nom du FRAME_COUNT
}

/*
 * Animation artifice2
 */

int read_gamelle_j2(unsigned long speed_frame){
  
/* Read matrix picture */

if (frame == 0 or (last_frame + speed_frame)< millis()){   
  for(int y=0; y<32; y++){ 
  
              for (int x=0; x<64; x++){
                int pix= y*64+x;
                unsigned int color= gamelle_j2_data[frame][pix];      // !!! Changer le nom de la data par le nom qui ce trouve dans le fichier
                //Serial.println(color,HEX);
                matrix.drawPixel(x, y, convert12to16(color));
              
               uint32_t color_32 = convert_16bit_color(color);
               edge_color(x,y,color_32);                        // Anime les rubans led

              }                                
   }
                 
   last_frame=millis();
   frame++;
   strip1.show();
   strip2.show();
   
}
if (frame>=GAMELLE_J2_FRAME_COUNT){if(!bitRead(statut_animation,LOOP)){bitSet(statut_animation,FINISH);}frame=0;} // !!! Utiliser le nom du FRAME_COUNT
}


/*
 * Animation water
 */

int read_intro(unsigned long speed_frame){
  
/* Read matrix picture */

if (frame== 0 or (last_frame + speed_frame)< millis()){   
  for(int y=0; y<32; y++){ 
  
              for (int x=0; x<64; x++){
                int pix= y*64+x;
                unsigned int color= intro_data[frame][pix];                    // Mettre à jour le nom en fonction de l'animation
                //Serial.println(color,HEX);
                matrix.drawPixel(x, y, convert12to16(color));
                uint32_t color_32 = convert_16bit_color(color);  
                edge_color(x,y,color_32);                                     // anime les rubans LED
                 
                }
   }
   last_frame=millis();
   frame++;
   strip1.show();
   strip2.show();
}
if (frame>=INTRO_FRAME_COUNT){if(!bitRead(statut_animation,LOOP)){bitSet(statut_animation,FINISH);}frame=0;}  // Mettre à jour le nom du FRAME_COUNT

}
