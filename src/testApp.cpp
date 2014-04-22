#include "testApp.h"


vector<ofPtr<hzChara> > charas;
hzChara *currentChara;

hzPlayer *player;
hzTextArea *textArea;



int move(lua_State* L)
{
  int dx = luaL_checkint(L, -2);
  int dy = luaL_checkint(L, -1);

  currentChara->move(dx, dy);

  return 0; //戻り値の数を指定
}

int say(lua_State* L)
{
  string text = luaL_checkstring(L, -1);
  textArea->setText(text);
}


//--------------------------------------------------------------
void testApp::setup(){
  ofSetFrameRate(60);


  font.loadFont("togoshi-mono.ttf", 12);

  ofLog() << "getLineHeight:" << font.getLineHeight() << endl;
  ofLog() << "getSpaceSize:" << font.getSpaceSize() << endl;

  // Luaを開く
    L = luaL_newstate();

    // Lua標準関数を使う
    luaL_openlibs(L);

    // Luaファイルを読み込む
    if(luaL_loadfile(L, "data\\base.lua") || lua_pcall(L, 0, 0, 0)) {
        perror(lua_tostring(L, -1));
    }

    if(luaL_loadfile(L, "data\\moves.lua") || lua_pcall(L, 0, 0, 0)) {
        perror(lua_tostring(L, -1));
    }
    if(luaL_loadfile(L, "data\\events.lua") || lua_pcall(L, 0, 0, 0)) {
        perror(lua_tostring(L, -1));
    }

    //move関数をcmove関数としてLuaに登録
    lua_register(L, "cmove", move);
    lua_register(L, "csay", say);


  // キャラ情報読み込み
  ofxJSONElement json;
  json.open("charas.json");

  eventRunning = false;

  for(int i=0;i<json.size();i++) {
    string charachip = json[i]["graphic"]["charachip"].asString();
    int    width     = json[i]["graphic"].isMember("width")  ? json[i]["graphic"]["width"].asInt()  : 32;
    int    height    = json[i]["graphic"].isMember("height") ? json[i]["graphic"]["height"].asInt() : 48;
    int    xi        = json[i]["graphic"]["xi"].asInt() * 3;
    int    yi        = json[i]["graphic"]["yi"].asInt() * 4;
    int    x         = json[i]["x"].asInt();
    int    y         = json[i]["y"].asInt();
    string moveScript    = json[i]["move"].asString();
    string eventScript    = json[i]["event"].asString();

    ofPtr<hzSprite> sprite(new hzSprite(charachip, width, height, xi, yi));
    ofPtr<hzChara> chara(new hzChara(sprite,x, y, moveScript, eventScript, L));
    charas.push_back(chara);
  }

  json.open("player.json");
  string charachip = json["graphic"]["charachip"].asString();
  int    width     = json["graphic"].isMember("width")  ? json["graphic"]["width"].asInt()  : 32;
  int    height    = json["graphic"].isMember("height") ? json["graphic"]["height"].asInt() : 48;
  int    xi        = json["graphic"]["xi"].asInt() * 3;
  int    yi        = json["graphic"]["yi"].asInt() * 4;
  int    x         = json["x"].asInt();
  int    y         = json["y"].asInt();

  ofPtr<hzSprite> sprite(new hzSprite(charachip, width, height, xi, yi));
  player = new hzPlayer(sprite,x, y, &cursor);

  currentChara = charas[0].get();;

  textArea = new hzTextArea(&font, "○○○○○○○○○○\n○○○○○○○○○○\n○○○○○○○○○○\n○○○○○○○○○○");
  textArea->setPosition(ofPoint(200,200,0));


  // tiled map editor
  Tmx::Map *map = new Tmx::Map();
	map->ParseFile("data/example.tmx");

	if (map->HasError()) {
		printf("error code: %d\n", map->GetErrorCode());
		printf("error text: %s\n", map->GetErrorText().c_str());

		system("PAUSE");

	}
	// Iterate through the tilesets.
	for (int i = 0; i < map->GetNumTilesets(); ++i) {
		printf("                                    \n");
		printf("====================================\n");
		printf("Tileset : %02d\n", i);
		printf("====================================\n");

		// Get a tileset.
		const Tmx::Tileset *tileset = map->GetTileset(i);

		// Print tileset information.
		printf("Name: %s\n", tileset->GetName().c_str());
		printf("Margin: %d\n", tileset->GetMargin());
		printf("Spacing: %d\n", tileset->GetSpacing());
		printf("Image Width: %d\n", tileset->GetImage()->GetWidth());
		printf("Image Height: %d\n", tileset->GetImage()->GetHeight());
		printf("Image Source: %s\n", tileset->GetImage()->GetSource().c_str());
		printf("Transparent Color (hex): %s\n", tileset->GetImage()->GetTransparentColor().c_str());

		ofImage tilesetImage;
		tilesetImage.loadImage(tileset->GetImage()->GetSource());
		int tileWidth   = map->GetTileWidth();
		int tileHeight  = map->GetTileHeight();
		int spacing     = tileset->GetSpacing());
		int margin      = tileset->GetMargin());
		string transparentColor = tileset->GetImage()->GetTransparentColor();

		background.loadImage(tileset->GetImage()->GetSource());

		if (tileset->GetTiles().size() > 0) {
			// Get a tile from the tileset.
			const Tmx::Tile *tile = *(tileset->GetTiles().begin());

			// Print the properties of a tile.
			std::map< std::string, std::string > list = tile->GetProperties().GetList();
			std::map< std::string, std::string >::iterator iter;
			for (iter = list.begin(); iter != list.end(); ++iter) {
				printf("%s = %s\n", iter->first.c_str(), iter->second.c_str());
			}
		}
	}

	// Iterate through the layers.
	for (int i = 0; i < map->GetNumLayers(); ++i) {
		printf("                                    \n");
		printf("====================================\n");
		printf("Layer : %02d/%s \n", i, map->GetLayer(i)->GetName().c_str());
		printf("====================================\n");

		// Get a layer.
		const Tmx::Layer *layer = map->GetLayer(i);

		for (int y = 0; y < layer->GetHeight(); ++y) {
			for (int x = 0; x < layer->GetWidth(); ++x) {
				// Get the tile's id.
				printf("%03d", layer->GetTileId(x, y));

				// Find a tileset for that id.
				const Tmx::Tileset *tileset = map->FindTileset(layer->GetTileId(x, y));
				if (layer->IsTileFlippedHorizontally(x, y)){
					printf("h");
				}else{
					printf(" ");
				}
				if (layer->IsTileFlippedVertically(x, y)){
					printf("v");
				}else{
					printf(" ");
				}
				if (layer->IsTileFlippedDiagonally(x, y)){
					printf("d ");
				} else {
					printf("  ");
				}
			}

			printf("\n");
		}
	}

	printf("\n\n");

	// Iterate through all of the object groups.
	for (int i = 0; i < map->GetNumObjectGroups(); ++i) {
		printf("                                    \n");
		printf("====================================\n");
		printf("Object group : %02d\n", i);
		printf("====================================\n");

		// Get an object group.
		const Tmx::ObjectGroup *objectGroup = map->GetObjectGroup(i);

		// Iterate through all objects in the object group.
		for (int j = 0; j < objectGroup->GetNumObjects(); ++j) {
			// Get an object.
			const Tmx::Object *object = objectGroup->GetObject(j);

			// Print information about the object.
			printf("Object Name: %s\n", object->GetName().c_str());
			printf("Object Position: (%03d, %03d)\n", object->GetX(), object->GetY());
			printf("Object Size: (%03d, %03d)\n", object->GetWidth(), object->GetHeight());

			// Print Polygon points.
			const Tmx::Polygon *polygon = object->GetPolygon();
			if (polygon != 0) {
				for (int i = 0; i < polygon->GetNumPoints(); i++) {
					const Tmx::Point &point = polygon->GetPoint(i);
					printf("Object Polygon: Point %d: (%d, %d)\n", i, point.x, point.y);
				}
			}

			// Print Polyline points.
			const Tmx::Polyline *polyline = object->GetPolyline();
			if (polyline != 0) {
				for (int i = 0; i < polyline->GetNumPoints(); i++) {
					const Tmx::Point &point = polyline->GetPoint(i);
					printf("Object Polyline: Point %d: (%d, %d)\n", i, point.x, point.y);
				}
			}
		}
	}

	delete map;
}

//--------------------------------------------------------------
void testApp::update(){
  // リサイズ無効化
  int w = ofGetWidth();
	int h = ofGetHeight();
	if(w != 640 || h != 480) ofSetWindowShape(640, 480);

  player->update(eventRunning);
  for(ofPtr<hzChara> chara : charas) {
    chara->update(eventRunning);
  }


  // イベント起動
  if(cursor.isBtn1() && !eventRunning) {
    for(ofPtr<hzChara> chara : charas) {
      if(chara->getPos().distance(player->getPos()) < 48) {
        ofLog() << chara->getEventScript() << endl;
        eventRunning = true;
        eventThreadIdx = addLuaThread(L, &event, chara->getEventScript());
      }
    }
  }

  if(eventRunning) {
    if(event != NULL) {
      lua_resume(event, NULL, 0);
      if (lua_type(event, -1) == LUA_TSTRING) {
        deleteLuaThread(L, &event, eventThreadIdx);
        eventRunning = false;
      }
    }
  }

}

//--------------------------------------------------------------
void testApp::draw(){
  ofBackground(100,100,100);

  ofSetColor(255,255,255);
  background.draw(0,0);

  // グリッド描画
  for(int y = 0; y < ofGetHeight(); y+= 10) {
      if(y % 100 == 0)ofSetColor(255, 255, 255);
      else ofSetColor(150, 150, 150);
    ofLine(0, y, ofGetWidth(), y);
  }
  for(int x = 0; x < ofGetWidth(); x+= 10) {
    if(x % 100 == 0)ofSetColor(255, 255, 255);
    else ofSetColor(150, 150, 150);
    ofLine(x, 0, x, ofGetHeight());
  }

  textArea->draw();

  ofSetColor(255,255,255);
  player->draw();
  for(ofPtr<hzChara> chara : charas) {
    chara->draw();
  }



//  chara.drawSubsection(pos.x, pos.y, 32, 48,xi * 32, yi * 48);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
  cursor.keyPressed(key);

  /*
  if(key == OF_KEY_UP        ) up    = true;
  else if(key == OF_KEY_DOWN ) down  = true;

  if(key == OF_KEY_RIGHT     ) right = true;
  else if(key == OF_KEY_LEFT ) left  = true;
    */
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
  cursor.keyReleased(key);
  /*
  if(key == OF_KEY_UP        ) up    = false;
  else if(key == OF_KEY_DOWN ) down  = false;

  if(key == OF_KEY_RIGHT     ) right = false;
  else if(key == OF_KEY_LEFT ) left  = false;
    */
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
