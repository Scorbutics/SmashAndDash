#include <iostream>
#include <string>
#include <fstream>
#include <SDL2/SDL_image.h>

#include "IDs.h"
#include "ScriptRead.h"
#include "GestionEvents.h"
#include "Scrolling.h"
#include "DialogMenu.h"
#include "ChargementImages.h"
#include "MouvementsMob.h"
#include "SavegameManager.h"

using namespace std;

std::string InterpretValue(std::string v);

int ScriptRead(string name)
{
	//Sécurité pour éviter la récursivité
	static bool active = false;
	WGameCore& wScreen = WGameCore::getInstance();

	if (!active)
	{
		active = true;
		
		World& w = wScreen.getWorld();
		string extendedName;
		ifstream fscript(name.c_str());
		extendedName = name + "_" + wScreen.getWorld().getName();
		ofstream fwscript, scriptList;
		string cmd;

		
		if (fscript.fail())
		{
			TCHAR NPath[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, NPath);

			fscript.open((string(NPath) + "\\" + name).c_str());
			if (fscript.fail())
				clog << "ERREUR : Pas de chance ! Impossible d'ouvrir " + string(NPath) + "\\" + name << endl;
			
		}

 		if (IsScriptActivated(extendedName))
		{
			while(fscript >> cmd)
			{

				if(cmd == "bouger") //bouge un pnj
				{
					int idtype, id, speed;
					string dir;

					fscript >> idtype;
					fscript >> id;
					fscript >> dir;
					fscript >> speed;

					for(unsigned int j = 0; j < dir.size(); j++)
					{
						for(int i = 0; i < (TAILLEBLOC+0.5*speed)/speed; i+=((speed+1)/2))
						{
							wScreen.getEntityFactory().getNPC(idtype, id)->reset();
							wScreen.getEntityFactory().getNPC(idtype, id)->applyForce(dir[j] - '0', (float)(100*speed));
							wScreen.getEntityFactory().getNPC(idtype, id)->setDirection(dir[j] - '0');
							wScreen.eventUpdate(true);
							wScreen.graphicUpdate();

							wScreen.flip();
							SDL_Delay(30);
						}
					}

				}
				else if(cmd == "message")
				{
					string fname, texte, buf;
					fscript >> fname;
					getline(fscript, texte);
					SDL_Rect menuPos;

					menuPos.x = 0;
					menuPos.y = wScreen.getHeight() - TAILLEBLOCFENETRE*4;
					menuPos.w = wScreen.getWidth()/2;
					menuPos.h = TAILLEBLOCFENETRE*4;

					DialogMenu menu(texte, "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22);

					menuPos.x += TAILLEBLOCFENETRE;
					menuPos.y -= TAILLEBLOCFENETRE*5;
					menuPos.w = 4*TAILLEBLOCFENETRE;
					menuPos.h = 5*TAILLEBLOCFENETRE;

					if(fname != "f")
					{
						if(atoi(fname.c_str()) >= 0)
							buf = fname;
						else
							buf = "pnj" + intToStr(abs(atoi(fname.c_str()))) ;
					}

					DialogMenu imgDial("", (fname != "f" ? "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Facesets"FILE_SEPARATOR"" + buf + ".png" : ""), "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22);
					KeyInput* in = wScreen.getInputListener().getKeyInput();

					while (menu.getX(menu.getLines() - 1) < menu.getWLine(menu.getLines() - 1))
					{
						wScreen.eventUpdate(true);
						wScreen.graphicUpdate();
						menu.display();
						if(fname != "f")
							imgDial.display();
                    
						wScreen.flip();
						SDL_Delay(30);
					}

					while (!(in->getKeyState(SDL_SCANCODE_RETURN) || in->getKeyState(SDL_SCANCODE_ESCAPE)))
					{
						wScreen.eventUpdate(true);
						wScreen.graphicUpdate();
						menu.display();
						if (fname != "f")
							imgDial.display();

						wScreen.flip();
						SDL_Delay(30);
					}

					wScreen.getInputListener().getKeyInput()->resetAll();
					//menu.pause();


				}
				else if (cmd == "choix")
				{
					string fname, texte, buf, var;
					fscript >> var;
					fscript >> fname;
					getline(fscript, texte);
					SDL_Rect menuPos, choicePos;

					menuPos.x = 0;
					menuPos.y = wScreen.getHeight() - TAILLEBLOCFENETRE * 4;
					menuPos.w = wScreen.getWidth() / 2;
					menuPos.h = TAILLEBLOCFENETRE * 4;
					
					choicePos = menuPos;
					choicePos.x += menuPos.w;
					choicePos.w = TAILLEBLOCFENETRE*(0.5);
					choicePos.h = TAILLEBLOCFENETRE * 3;
					choicePos.y -= choicePos.h;
					
					if (choicePos.x >= wScreen.getWidth())
						choicePos.x = wScreen.getWidth() - choicePos.w;
					
					DialogMenu menu(texte, "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22);
					DialogMenu choice("Entrée:Oui¤Echap:Non", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", choicePos, 22);
					
					menuPos.x += TAILLEBLOCFENETRE;
					menuPos.y -= TAILLEBLOCFENETRE * 5;
					menuPos.w = 4 * TAILLEBLOCFENETRE;
					menuPos.h = 5 * TAILLEBLOCFENETRE;

					if (fname != "f")
					{
						if (atoi(fname.c_str()) >= 0)
							buf = fname;
						else
							buf = "pnj" + intToStr(abs(atoi(fname.c_str())));
					}

					DialogMenu imgDial("", (fname != "f" ? "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Facesets"FILE_SEPARATOR"" + buf + ".png" : ""), "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22);

					while (menu.getX(menu.getLines() - 1) < menu.getWLine(menu.getLines() - 1))
					{
						wScreen.eventUpdate(true);
						wScreen.graphicUpdate();
						menu.display();
						if (fname != "f")
							imgDial.display();

						wScreen.flip();
						SDL_Delay(30);
					}
					
					do
					{
						wScreen.eventUpdate(true);
						wScreen.graphicUpdate();
						menu.display();
						choice.display();
						if (fname != "f")
							imgDial.display();

						wScreen.flip();
						SDL_Delay(30);

					} while (!wScreen.getInputListener().getKeyInput()->getKeyState(SDL_SCANCODE_RETURN) && !wScreen.getInputListener().getKeyInput()->getKeyState(SDL_SCANCODE_ESCAPE));

					SetValueFromVarOrSwitchNumber(var, wScreen.getInputListener().getKeyInput()->getKeyState(SDL_SCANCODE_RETURN) ? "1" : "0");
					wScreen.getInputListener().getKeyInput()->resetAll();
					
					
				}
				else if(cmd == "fin") //ne joue plus ce script jusqu'au changement de carte ou redemmarage
				{
					fscript.close();

					scriptList.open(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + wScreen.getSavegameManager().getSaveName() + FILE_SEPARATOR"tmpscripts.data").c_str(), ios::app);
					scriptList << extendedName << endl;
					scriptList.close();
					break;
				}
				else if(cmd == "soigner")
				{
					int value;
					unsigned int index;
					fscript >> index;
					fscript >> value;
					if(index >= 0 && index < wScreen.getPokemonManager().getPokemonTeamSize())
						wScreen.getPokemonManager().getPokemon(index)->setHP(wScreen.getPokemonManager().getPokemon(index)->getHp() + value);

				}
				else if(cmd == "stop") //ne joue plus JAMAIS ce script pour la sauvegarde
				{
					fscript.close();

					scriptList.open(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + wScreen.getSavegameManager().getSaveName() + FILE_SEPARATOR"scripts.data").c_str(), ios::app);
					scriptList << extendedName << endl;
					scriptList.close();
					break;

				}
				else if(cmd == "si") // les conditions. ex : si [3] > 2 testera si la valeur contenue par la variable 3 est superieur strictement a 2
				{
					int ifEnd = 1, num1, num2;
					string varNumber, op, valeur, line;

					fscript >> varNumber;
					fscript >> op;
					fscript >> valeur;

					num1 = GetValueFromVarOrSwitchNumber(varNumber);
					num2 = GetValueFromVarOrSwitchNumber(valeur);


					if(!((op == "==" && num1 == num2) || (op == "<" && num1 < num2) || (op == ">" && num1 > num2) || (op == "<=" && num1 <= num2) || (op == ">=" && num1 >= num2)))
					{

						while(ifEnd > 0 && getline(fscript, line))
						{
							if(line == "si")
								ifEnd++;
							else if(line == "finsi" || line == "sinon")
								ifEnd--;
						}

					}

				}
				else if(cmd == "sinon")
				{
					int ifEnd = 1;
					string line;

						while(ifEnd > 0 && getline(fscript, line))
						{
							if(line == "si")
								ifEnd++;
							else if(line == "finsi")
								ifEnd--;
						}
				}
				else if(cmd == "affecter") // Ex: affecter 300 [1]
				{
					string valeur, varNumber;

					fscript >> valeur;
					fscript >> varNumber;

					
					SetValueFromVarOrSwitchNumber(varNumber, InterpretValue(valeur));
				}
				else if(cmd == "mod_texte") //modifie le texte d'une entite
				{

				}
				else if(cmd == "combat")
				{
					int idtype;

					fscript >> idtype;

				}
				else if(cmd == "direction") //change de direction une entite
				{
					int number, id;
					int dir;

					fscript >> id;
					fscript >> number;
					fscript >> dir;

					wScreen.getEntityFactory().getNPC(id, number)->setDirection(dir);

				}
				else if(cmd == "animation") //exclamation sur une entite
				{
					int idtype, id, id_anim;
					SDL_Rect posAnim;

					fscript >> idtype;
					fscript >> id;
					fscript >> id_anim;

					if (wScreen.getEntityFactory().getNPC(idtype, id) != NULL)
					{
						posAnim = wScreen.getEntityFactory().getNPC(idtype, id)->getHitboxCenterPos();
						posAnim.y -= TAILLEBLOC/2;
						posAnim.x -= TAILLEBLOC / 2;
						wScreen.getSpriteAnimationManager().play(SPRITEBANK_ANIMATION, id_anim, posAnim, 1, 200);
					}

				}
				else if(cmd == "eboulement") //fait une animation d'éboulement à l'écran
				{
					int duree = 0;

					fscript >> duree;

					wScreen.getParticleManager().playCrumbling(1, 30, 0.25, duree);
				}
				else if(cmd == "tremblement") //fait trembler l'ecran
				{
					unsigned intensity, duration;

					fscript >> duration;
					fscript >> intensity;

					wScreen.getShakerManager().shake(intensity, duration);

				}
				else if (cmd == "translation_camera")
				{
					unsigned int duree = 0, t0 = 0;
					int speedx, speedy;
					SDL_Rect tmpRelativeOrigin;
					
					fscript >> duree;
					fscript >> speedx;
					fscript >> speedy;

					tmpRelativeOrigin = wScreen.getORel();
					wScreen.activeScrolling(false);

					t0 = SDL_GetTicks();
			
					while (SDL_GetTicks() - t0 < duree)
					{
						tmpRelativeOrigin.x -= speedx;
						tmpRelativeOrigin.y -= speedy;

						wScreen.graphicUpdate();
						wScreen.eventUpdate(true);
						wScreen.setORel(tmpRelativeOrigin);

						wScreen.flip();
						SDL_Delay(20);

					}

					wScreen.activeScrolling(true);
				}
				else if(cmd == "attendre") //attendre pendant une certaine durée dans un script
				{
					unsigned int duree = 0, t0 = 0;

					fscript >> duree;

					t0 = SDL_GetTicks();

					while(SDL_GetTicks() - t0 < duree)
					{

						wScreen.graphicUpdate();
						wScreen.eventUpdate(true);
                    
						wScreen.flip();
						SDL_Delay(20);

					}

				}
				else if(cmd == "bloquer_perso")
				{
					int id, number, moving;

					fscript >> id;
					fscript >> number;
					fscript >> moving;

					wScreen.getEntityFactory().getNPC(id, number)->setMoving(!moving);

				}
				else if(cmd == "cacher_perso")
				{
					int id, number, hiding;

					fscript >> id;
					fscript >> number;
					fscript >> hiding;

					wScreen.getEntityFactory().getNPC(id, number)->setVisible(!hiding);

				}
				else if (cmd == "suivre_perso")
				{
					int id, number, id_target, number_target, flag;

					fscript >> id;
					fscript >> number;
					fscript >> id_target;
					fscript >> number_target;
					fscript >> flag;

					wScreen.getEntityFactory().getNPC(id, number)->followEntity(flag ? wScreen.getEntityFactory().getNPC(id_target, number_target) : NULL);
				}
				else if(cmd == "shop")
				{
					wScreen.getGUI().getWindowShop()->reset();
					wScreen.getGUI().getWindowShop()->hide(false);
					wScreen.waitQuit(wScreen.getGUI().getWindowShop()->getShopBar());
				}
				else if(cmd == "cacher_interface")
				{
					int b;

					fscript >> b;

					wScreen.getGUI().getToolbar()->hide(b != 0);
					wScreen.getGUI().hide(b != 0);
				}
				else if(cmd == "teleporter")
				{
					int id, number;
					string param;

					fscript >> id;
					fscript >> number;
					fscript >> param;
				
					if(id != 0)
					{
						int x, y;
						x = atoi(param.substr(0, param.find_first_of(':')).c_str());
						y = atoi(param.substr(param.find_first_of(':')+1, param.size()).c_str());

						wScreen.getEntityFactory().getNPC(id, number)->teleport(x*TAILLEBLOC, y*TAILLEBLOC);
					}
					else
						TeleportHeroToMap(param);
					


				}
				else if (cmd == "script")
				{
					string scriptName;
					getline(fscript, scriptName);
					active = false;
					
					fscript.close();
					fwscript.close();
					ScriptRead(scriptName.substr(1, scriptName.length()));
					
					active = true;
				}
				else if (cmd == "cinematique")
				{
					unsigned int frameNumber, delay, id_anim, duration;
					bool continuer = true;
					SDL_Rect pos, menuPos;
					string message;
					SpriteAnimation* cinematic;
					KeyInput* in = wScreen.getInputListener().getKeyInput();

					menuPos.x = 0;
					menuPos.y = wScreen.getHeight() - TAILLEBLOCFENETRE * 4;
					menuPos.w = wScreen.getWidth();
					menuPos.h = TAILLEBLOCFENETRE * 4;

					fscript >> id_anim;
					fscript >> delay;
					fscript >> duration;
					fscript >> frameNumber;
					fscript >> pos.x;
					fscript >> pos.y;
	
					getline(fscript, message);
					
					pos.x -= wScreen.getORel().x;
					pos.y -= wScreen.getORel().y;

					DialogMenu messageBox(message, "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 21);
					
					wScreen.getSpriteAnimationManager().reset();
					
					cinematic = wScreen.getSpriteAnimationManager().play(SPRITEBANK_ANIMATION, id_anim, pos, -1, -1, frameNumber, delay);
						
					while (continuer)
					{
						SDL_RenderClear(wScreen.getRenderer());
						wScreen.getSpriteAnimationManager().refresh();
						wScreen.getInputListener().refresh(true);
						if (in->getKeyState(SDL_SCANCODE_SPACE) || in->getKeyState(SDL_SCANCODE_RETURN) || in->getKeyState(SDL_SCANCODE_ESCAPE))
							continuer = false;

						messageBox.refresh();
						messageBox.display();
						wScreen.flip();
						SDL_Delay(20);
					}
					in->resetAll();
					wScreen.getSpriteAnimationManager().reset();
				}
				else if (cmd == "tuer")
				{
					int id, number;
					string param;

					fscript >> id;
					fscript >> number;
					
					wScreen.getEntityFactory().remove(id, number);
				}
				else if (cmd == "ajouter_pokemon")
				{
					int id;

					fscript >> id;

					wScreen.getPokemonManager().add(id);
				}
				else if (cmd == "sortir_pokemon")
				{
					int index;
					unsigned int duree = 2000, t0 = 0;

					fscript >> index;
					SDL_Rect launchPos = wScreen.getEntityFactory().getTrainer()->getCenterPos();
					launchPos.y += TAILLEBLOC;

					wScreen.getPokeball().launch(wScreen.getEntityFactory().getTrainer(), launchPos);
					
					t0 = SDL_GetTicks();

					while (SDL_GetTicks() - t0 < duree)
					{

						wScreen.graphicUpdate();
						wScreen.eventUpdate(true);

						wScreen.flip();
						SDL_Delay(20);

					}
					wScreen.getEntityFactory().addNPC(wScreen.getPokemonManager().getPokemon(index)->getID(), launchPos, "1");

				}

				wScreen.getInputListener().getKeyInput()->resetAll();
			}
		}

		fscript.close();
		active = false;
	}
	
    return 0;
}


int GetValueFromVarOrSwitchNumber(string varNumber)
{
    int num;
	WGameCore& wScreen = WGameCore::getInstance();

    if(varNumber[0] == '{' && varNumber[varNumber.size()-1] == '}')
    {
        if(wScreen.getSavegameManager().getGameSwitch(atoi(varNumber.substr(1, varNumber.size()-2).c_str()) - 1))
            num = 1;
        else
            num = 0;
    }
    else if(varNumber[0] == '[' && varNumber[varNumber.size()-1] == ']')
        num = wScreen.getSavegameManager().getGameVariable(atoi(varNumber.substr(1, varNumber.size()-2).c_str()) - 1);
    else if(varNumber == "true")
        num = 1;
    else if(varNumber == "false")
        num = 0;
    else
        num = atoi(varNumber.c_str());

    return num;
}

void SetValueFromVarOrSwitchNumber(string varNumber, string value)
{
	WGameCore& wScreen = WGameCore::getInstance();

    if(varNumber[0] == '{' && varNumber[varNumber.size()-1] == '}')
    {
        if(value == "true")
            wScreen.getSavegameManager().setGameSwitch(atoi(varNumber.substr(1, varNumber.size()-2).c_str()) - 1, true);
        else
            wScreen.getSavegameManager().setGameSwitch(atoi(varNumber.substr(1, varNumber.size()-2).c_str()) - 1, false);
    }
    else if(varNumber[0] == '[' && varNumber[varNumber.size()-1] == ']')
        wScreen.getSavegameManager().setGameVariable(atoi(varNumber.substr(1, varNumber.size()-2).c_str()) - 1, atoi(value.c_str()));

}

std::string InterpretValue(std::string v)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int dotPos = v.find_first_of('.');

	if (dotPos != std::string::npos)
	{
		if (v.find("dresseur") == 0)
		{
			Player* p = wScreen.getEntityFactory().getTrainer();
			if (v.find("pv", dotPos) != std::string::npos)
				return intToStr(p->getHp());
			else if (v.find("posx", dotPos) != std::string::npos)
				return intToStr(p->getPos().x);
			else if (v.find("posy", dotPos) != std::string::npos)
				return intToStr(p->getPos().y);
			
		}
	}
	
	return v;
}

bool IsScriptActivated(const string& scriptName)
{
    string s;
	WGameCore& wScreen = WGameCore::getInstance();
	ifstream scriptList(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + wScreen.getSavegameManager().getSaveName() + FILE_SEPARATOR"scripts.data").c_str(), ios::app);
	ifstream tmpScriptList(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + wScreen.getSavegameManager().getSaveName() + FILE_SEPARATOR"tmpscripts.data").c_str(), ios::app);
	
	if (scriptList.fail())
		return true;

    do
        scriptList >> s;
    while(s != scriptName && !scriptList.eof());


    if(s == scriptName)
        return false;
	else
	{
		s = "";
		if (tmpScriptList.fail())
			return true;

		do
			tmpScriptList >> s;
		while (s != scriptName && !tmpScriptList.eof());

		return !(s == scriptName);
	}
}

/*
void ScriptsActiver(string worldName)
{
    ofstream scriptList;
	scriptList.open(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + wScreen.getSavegameManager().getSaveName() + FILE_SEPARATOR"scripts.data").c_str(), ios::trunc);
    scriptList.close();
}
*/