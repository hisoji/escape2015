#pragma once
#include<Siv3D.hpp>

void SetRectBottom(Rect& r, Point p){
	r.setPos(p - Point(r.w / 2, r.h));
}

class MyCamera
{
public:
	MyCamera(){}

	void update(const Point& playerPos, const Point& mapSize){
		Point pos = -Point((1280 - 128) / 2, 720 / 5 * 3) + playerPos;/*プレイヤー位置からカメラを引いたもの*/

		//以下カメラの位置調整(左上基準?)
		if (pos.x<0){
			pos.x = 0;
		}
		if (pos.x + 1280 > mapSize.x){
			pos.x = mapSize.x - 1280;
		}
		if (pos.y<0){
			pos.y = 0;
		}
		if (pos.y + 720>mapSize.y){
			pos.y = mapSize.y - 720;
		}

		m_camerapos = pos;
	}

	Point getCameraPos()const{
		return m_camerapos;
	}
	void debug()const{
		const Point m = Mouse::Pos();
		FontAsset(L"debugfont").draw(Format(m + m_camerapos), m, Palette::Black);
	}
private:
	
	Point m_camerapos = Point(0,0);
};

enum class FloorType{
	NormalFloor,
	PassFloor,
	DamageFloor,
	DeathFloor,
	Null
};

class Map{
public:
	Map(){

	}
	void init(String mapName,String stageName){
		loadStage(mapName, stageName);;
	}
	void update(){}
	void draw(const Point& offset = { 0, 0 })const{
		m_texMap(offset.x, offset.y, SCREEN_SIZE.x, SCREEN_SIZE.y).draw();
	}

	//メインから戻る時の処理
	void updateReturn(){
		SoundAsset(m_bgmAsset).stop();
	}

	void loadStage(const String& mapName, const String& stageName){
		m_imageTerrain = Image(L"data/Elis/Map/" + mapName + L"/" + stageName + L"_terrain.png");
		m_texMap = Texture(L"data/Elis/Map/" + mapName + L"/" + stageName + L"_map.png");
		//m_texMap = Texture(L"data/Elis/Map/" + mapName + L"/" + stageName + L"_terrain.png");
		m_bgmAsset = L"ELBGMrain";

		if (!m_imageTerrain){
			LOG_ERROR(L"テラインが読み込まれていません Map::loadStage\n");
		}
		if (!m_texMap){
			LOG_ERROR(L"マップ画像が読み込まれていません Map::loadStage\n");
		}

		SoundAsset(m_bgmAsset).play();
	}
	bool isFloor(int x, int y)const{
		return m_imageTerrain[y][x] != Palette::White;
	}
	FloorType getFloorType(int x, int y)const
	{
		Color col = m_imageTerrain[y][x];

		if (col == Palette::Black)
		{
			return FloorType::NormalFloor;
		}
		else if (col == Palette::Blue)
		{
			return FloorType::PassFloor;
		}
		else if (col == Palette::Red)
		{
			return FloorType::DeathFloor;
		}
		else
		{
			return FloorType::Null;
		}
	}

	bool intersectBody(const Point& foot, const Point& head,const FloorType& floorType)const//通常床と身体の接触判定
	{
		for (int i = 1; i<foot.y - head.y; ++i)
		{
			if (isFloor(foot.x, foot.y - i) && getFloorType(foot.x, foot.y - i) == floorType)
			{
				return true;
			}
		}
		return false;
	}

	Point getMapSize()const{
		return m_imageTerrain.size;
	}

private:
	Texture m_texMap;
	Image m_imageTerrain; //地形衝突用の画像
	String m_bgmAsset;
};

class Player{
public:
	enum class Face {
		Left,
		Right
	};
	enum class ElisState{
		Staying,
		Running,
		Jumping,
		Falling,
		Fire,
		Damaged,
		Dead,
	};

	void init(Point playerPos = { 100, 500 }){
		elisRect.setCenter(playerPos);
	}
	void update(){
		nextVec = { 0, 0 };
		move();
		++m_frameCount;
	}
	void draw(const Point& camerapos = { 0, 0 })const{
		const int kAnimeTiming = 31;
		const Point pos = elisRect.pos - camerapos;
		const double x = (face == Face::Left) ? 0 : 128;
		const double y = 128;
		const double w = elisRect.w;
		const double h = elisRect.h;
		const int a = invincibled ? 155 : 255;
		switch (elisState)
		{
		case ElisState::Staying:
			TextureAsset(L"texElisNormal")(x, y*((m_frameCount / kAnimeTiming) % int(TextureAsset(L"texElisNormal").height / y)), w, h).draw(pos, Alpha(a));
			break;
		case ElisState::Running:
			TextureAsset(L"texElisRun")(x, y*((m_frameCount / kAnimeTiming) % int(TextureAsset(L"texElisNormal").height / y)), w, h).draw(pos, Alpha(a));
			break;
		case ElisState::Jumping:
			TextureAsset(L"texElisJump")(x, y, w, h).draw(pos, Alpha(a));
			break;
		case ElisState::Falling:
			TextureAsset(L"texElisJump")(x, y, w, h).draw(pos, Alpha(a));
			break;
		}
		Rect(pos,w,h).draw({ Palette::Black, 100 });
	}

	Point footCollisionPos()const{
		return elisRect.center + Point(0, elisRect.h / 2);
	}
	Point headCollisionPos()const{
		return Point(elisRect.center.x, elisRect.y + 50);
	}
	Point getPlayerPos()const {
		return elisRect.pos;
	}

	void moveChara(const Point& p){
		elisRect.moveBy(p);
	}

	Rect getRect(){
		return elisRect;
	}

	Point nextVec;//次に動くベクトル。maingameで使ってる

	struct JumpInfo{
		bool jumping = false;
		int m_jumpH = 0;
		const int kMaxJumpH = 27;
		const int kDrop = 1;
	};
	JumpInfo jumpInfo;

	Key keyRight = Input::KeyRight;
	Key keyLeft = Input::KeyLeft;
	Key keyDown = Input::KeyDown;
	Key keyUp = Input::KeyUp;

	bool invincibled = false;
	Rect elisRect = Rect(100, 500, 128, 128);
	Face face = Face::Right;
	ElisState elisState = ElisState::Staying;
	int m_speed = 0;
	int m_frameCount = 0;
	int m_gravity = 5;

	const int kMaxSpeed = 5;
	const int kMinSpeed = 0;
	const int kMaxGravity = 10;
	const int kMaxJumpH = 27;//ジャンプ初速度
	const int kDrop = 1;//ジャンプ速度の減少度兼重力加速度
	const int kGravity0 = 5;


private:
	void move(){
		ElisState next = ElisState::Staying;

		bool input = false;
		if (!(keyRight.pressed && keyLeft.pressed))//同時押し回避
		{
			if (keyRight.pressed)///////////////右に進むキー
			{
				input = true;
				face = Face::Right;
				m_speed = Min(m_speed + 1, kMaxSpeed);
				next = ElisState::Running;
			}
			if (keyLeft.pressed)////////////////左に進むキー
			{
				input = true;
				face = Face::Left;
				m_speed = Max(m_speed - 1, -kMaxSpeed);
				next = ElisState::Running;
			}
		}
		if (input == false){
			m_speed = Clamp(m_speed - 1, kMinSpeed, m_speed + 1);
		}
		nextVec += { m_speed, 0 };

		//jump
		if (keyUp.clicked && elisState != ElisState::Jumping){
			SoundAsset(L"sndElisJump").play();
			jumpInfo.m_jumpH = jumpInfo.kMaxJumpH;
			jumpInfo.jumping = true;
		}
		
		elisState = next;
	}	
};

struct Item{
	enum class Type{
		Key,
		JewelN,
	};
	static String GetItemAssetName(Item::Type type){
		switch (type){
		case Item::Type::Key:
			return L"texELMagicUpM";
		default:
			return L"texELMagicUpS";
		}
	}

	Item(Type t,Point pos,String map=L"house",String stage=L"stage00")
		:type(t),mapName(map),stageName(stage){
		assetName = GetItemAssetName(t);
		erased = false;
		switch (t){
		case Type::Key:
			rect.size = { 20, 20 };
			break;
		default:
			rect.size = { 20, 20 };
			break;
		}
		rect.setCenter(pos);
	}

	Type type;
	Rect rect;
	String assetName;
	bool erased;//消す時にtrue
	String mapName;
	String stageName;
};

class ItemManager{
public:
	void init(){
		allItems.clear();
		loadItem(L"house", L"stage00");
		loadItem(L"house", L"stage01");
		loadItem(L"house", L"stage02");
		loadItem(L"house", L"stage03");
		loadItem(L"house", L"stage04");
		loadItem(L"house", L"stage05");
	}

	void selectStage(String map, String stage){
		mapName = map;
		stageName = stage;
	}
	void update(){
		/*
		Erase_if(items, [](const Item& i){
			return i.erased;
		});
		*/
	}

	Optional<Item::Type> findItem(const Rect& playerRect = { 0, 0, 0, 0 }){
		for (Item& i : allItems){
			if (i.mapName == mapName && i.stageName == stageName && 
				i.erased == false && i.rect.intersects(playerRect))
			{
				i.erased = true;
				return i.type;
			}
		}
		return none;
	}

	void draw(const Point& offset = { 0, 0 })const{	
		for (const Item &item : allItems){
			if (item.mapName == mapName && item.stageName == stageName && 
				item.erased == false && isInCamera(item.rect.center, offset))
			{
				const Point p = item.rect.center - offset;
				TextureAsset(item.assetName).drawAt(p);
			}
		}
	}

	size_t getSize()const{
		return allItems.size();
	}

private:
	void loadItem(const String& mapName, const String& stageName){
		CSVReader csv(L"data/Elis/Map/" + mapName + L"/" + stageName + L".csv");
		if (csv){
			for (size_t y = 0; y < csv.rows; ++y){
				if (csv.get<String>(y, 0) == L"item"){
					if (csv.get<String>(y, 1) == L"for"){
						int n = csv.get<int>(y, 2);
						if (n == 0){//0除算を一時的に防ぐ
							n == 1;
						}
						const Point from = { csv.get<int>(y, 4), csv.get<int>(y, 5) };
						const Point to = { csv.get<int>(y, 7), csv.get<int>(y, 8) };
						const Point by = (to - from) / (n - 1);
						for (int i = 0; i < n; ++i){
							allItems.push_back({ Item::Type::JewelN, from + by*i, mapName, stageName });
						}
					}
					else{
						Point pos = { csv.get<int>(y, 1), csv.get<int>(y, 2) };
						allItems.push_back({ Item::Type::Key, pos, mapName, stageName });
					}
				}
			}
		}
		else{
			allItems.push_back({ Item::Type::Key, { 600, 600 } });
			allItems.push_back({ Item::Type::JewelN, { 800, 600 } });
		}
	}
	bool isInCamera(const Point& pos, const Point& camerapos)const
	{
		const int kXoffset = 300;
		const int kYoffset = 300;

		return pos.x > camerapos.x - kXoffset
			&& pos.x < camerapos.x + 1280 + kXoffset
			&& pos.y > camerapos.y - kYoffset
			&& pos.y <camerapos.y + 720 + kYoffset;
	}

	Array<Item> nowItems;
	Array<Item> allItems;
	String stageName;
	String mapName;
};


struct Door{
public:
	Door(){}
	Door(Point _pos,String nextStage,Point nextPoint)
		:hitRect(_pos,50,80),nextStageName(nextStage),nextMapPoint(nextPoint),openType(Item::Type::Key){
		SetRectBottom(hitRect, _pos);
	}
	void draw(const Point& p)const{
		if (isVisible){
			hitRect.movedBy(-p).draw({ Palette::Yellow, 128 });
		}
	}

	bool isTouchable(const Point& head, const Point& foot)const{
		int n = std::abs(foot.y - head.y);
		for (int i = 0; i <= n; ++i)
		{
			if (Point(foot.x, foot.y - i).intersects(hitRect))
			{
				return true;
			}
		}
		return false;
	}

	Rect hitRect;
	String nextStageName;
	Point nextMapPoint;
	Item::Type openType;
	bool isVisible = true;
};

class Flags{
public:
	enum Type{
		Started,
		Num
	};
	Flags(){
		for (size_t i = 0; i < Num; ++i){
			list.push_back(false);
		}
	}
	bool get(String str)const{
		return list[strToType(str)];
	}
	void set(String str, bool b){
		list[strToType(str)] = b;
	}
private:
	Type strToType(String str)const{
		if (str == L"started"){
			return Started;
		}
		return Num;//error
	}
	Array<bool> list;
};
Flags flags;

enum EventType
{
	TalkEvent,
	AutoEvent,
	AutoStart,
	Num,
};

struct EventInfo
{
	Rect eventArea;
	String filename;
	EventType type;
	String flagName = L"nothing";//条件となるフラグ名。nothingなら自動
	bool readed = false;//自動イベントが連続で表示されるのを防ぐための応急処置
	bool isVisible = true;
};

class EventManager
{
private:
	Array< EventInfo > m_events;
	bool m_isEventEnd = false;
	Font m_TalkFont = Font(25, Typeface::Default);
	int m_eventIndex = 0;

	CSVReader eventCSV;
	int csvPos = 0;

public:
	EventManager(){}

	//マップ内のイベントをロードする。マップに入った瞬間に呼ぶ。
	void loadEvent(String stageName)
	{
		m_events.clear();
		CSVReader csv(L"data/Elis/Map/house/" + stageName + L".csv");
		if (csv){
			for (int y = 0; y < csv.rows; ++y)
			{
				const String str = csv.get<String>(y, 0);
				if (str == L"event"){
					EventInfo item;
					item.filename = csv.get<String>(y, 1).lower();
					item.type = EventType::TalkEvent;
					item.eventArea = Rect(100, 128);
					SetRectBottom(item.eventArea, { csv.get<int>(y, 2), csv.get<int>(y, 3) });
					m_events.push_back(item);
				}
				else if (str == L"autoevent"){
					EventInfo item;
					item.filename = csv.get<String>(y, 1).lower();
					item.type = EventType::AutoEvent;
					item.eventArea = Rect(100, 128);
					SetRectBottom(item.eventArea, { csv.get<int>(y, 2), csv.get<int>(y, 3) });
					m_events.push_back(item);
				}
				else if (str == L"autostart"){//巨大な判定で強制的に触れさせる
					EventInfo item;
					item.filename = csv.get<String>(y, 1).lower();
					item.flagName = csv.get<String>(y, 2).lower();
					item.type = EventType::AutoStart;
					item.eventArea = Rect(0,0,SCREEN_SIZE);
					item.isVisible = false;
					m_events.push_back(item);
					
				}
			}
			return;
		}
	}

	//イベント処理(引数を追加していい。考えられるもの...プレイヤー位置、会話ボタンが押されたか)
	void update()
	{
		if (m_events[m_eventIndex].type == EventType::AutoStart){
			if (Input::KeyZ.clicked){
				++csvPos;
				if (csvPos >= eventCSV.rows){
					m_isEventEnd = true;
				}
			}
		}
		else{
			if (Input::KeyZ.clicked)
			{
				m_isEventEnd = true;
			}
		}
	}

	//イベント開始
	void startEvent()
	{
		m_isEventEnd = false;
	}

	//発生するイベントがあるか。trueならイベントモードに移る
	bool hasEvent(const Rect& playerRect, bool isClickedTalkButton)
	{
		bool flag = false;

		const auto setStart = [&](int i){
			flag = true;
			m_eventIndex = i;
		};

		for (size_t i = 0; i < m_events.size(); ++i)
		{
			if (m_events[i].eventArea.intersects(playerRect))
			{
				if (m_events[i].type == EventType::TalkEvent && isClickedTalkButton)
				{
					setStart(i);
					break;
				}
				else if (m_events[i].type == EventType::AutoEvent && !m_events[i].readed)
				{
					setStart(i);
					m_events[i].readed = true;
					break;
				}
				else if (m_events[i].type == EventType::AutoStart && flags.get(m_events[i].flagName) == false)
				{
					setStart(i);
					m_events[i].readed = true;
					eventCSV.open(L"data/Elis/Map/house/" + m_events[i].filename + L".csv");
					if (!eventCSV){
						LOG_ERROR(L"存在しないcsvです");
					}

					flags.set(m_events[i].flagName, true);
					break;
				}
			}
			else if (m_events[i].type == EventType::AutoEvent)//重なっているとき連続で出ないようにするための処置
			{
				m_events[i].readed = false;
			}
		}
		return flag;//false;//イベントが発生したらtrue,そうでないならfalse
	}

	//イベントが終わったか。trueなら通常のモードに移る
	bool isEnd(){
		return m_isEventEnd;//event終了ならtrueを返す
	}

	void drawCharacter(const Point& cameraOffset) const
	{
		for (size_t i = 0; i < m_events.size(); ++i)
		{
			if (m_events[i].isVisible){
				TextureAsset(L"texElisNormal")(0, 0, 128, 128).drawAt(m_events[i].eventArea.center - cameraOffset);

				Rect(m_events[i].eventArea.pos - cameraOffset, m_events[i].eventArea.size).draw({ Palette::Blue, 100 });
			}
		}
	}

	void drawEvent() const
	{
		const auto drawTalkWindow = [](){
			Rect(50, 50, 1180, 300).draw(Palette::Lightslategray);
			Rect(50, 50, 1180, 300).drawFrame(10.0, 0.0, Palette::Black);
		};

		if (m_events[m_eventIndex].type == EventType::AutoStart){
			drawTalkWindow();
			const String str = eventCSV.get<String>(csvPos, 0);
			const int maxChar = 29;
			m_TalkFont(str.substr(0, maxChar)).draw(100, 100, Palette::Black);
			if (str.length >= maxChar){
				m_TalkFont(str.substr(maxChar, maxChar)).draw(100, 150, Palette::Black);
			}
			if (str.length >= maxChar*2){
				m_TalkFont(str.substr(maxChar*2, maxChar)).draw(100, 200, Palette::Black);
			}
		}
		else{
			drawTalkWindow();
			m_TalkFont(m_events[m_eventIndex].filename).draw(100, 100, Palette::Black);
		}
		//LOG_ERROR(m_events[m_eventIndex].filename);
	}
};

class MainGame{
	enum class State{
		Playing,
		Event,
	};
public:
	void init(){
		const String mn = g_gameData.mapName;
		const String sn = g_gameData.stageName;
		nowMapName = mn;
		nowStageName = sn;
		player.init({ 200, 500 });
		map.init(mn,sn);
		itemManager.init();
		eventManager.loadEvent(sn);

		loadDoors(nowStageName);
	}
	void update(){
		switch (state){
		case State::Playing:
			if (mapMoving){
				const String ns = nextDoor.nextStageName;
				SetRectBottom(player.elisRect,nextDoor.nextMapPoint);
				nowStageName = ns;
				map.loadStage(nowMapName,ns);
				itemManager.selectStage(nowMapName, ns);
				eventManager.loadEvent(ns);
				loadDoors(ns);

				camera.update(player.getPlayerPos(), map.getMapSize());
				
				mapMoving = false;
			}
			else{
				player.update();
				if (Input::KeyEnter.clicked){
					map.updateReturn();
					ChangeScene(GameState::StageSelect);
				}
				intersectPlayertoMap();
				camera.update(player.getPlayerPos(), map.getMapSize());

				itemManager.update();
				if (auto p = itemManager.findItem(player.elisRect)){
					SoundAsset(L"sndELGetItem").playMulti();
					if (p.value() == Item::Type::JewelN){
						++itemCount;
					}
					else{
						havingItems.push_back(p.value());
					}
				}

				//何か発生イベントが起こるならイベント遷移
				if (eventManager.hasEvent(player.getRect(), Input::KeyZ.clicked)){
					state = State::Event;
					eventManager.startEvent();
				}

				for (Door& d : doors){
					if (d.hitRect.intersects(player.elisRect) && Input::KeyZ.clicked){
						nextDoor = d;
						mapMoving = true;
					}
				}
			}
			break;

		case State::Event:
			eventManager.update();

			//イベントが終わったら通常の状態に戻る
			if (eventManager.isEnd()){
				state = State::Playing;
			}
			break;
		}
	}
	void draw()const{
		const Point& p = camera.getCameraPos();
		map.draw(p);
		itemManager.draw(p);
		eventManager.drawCharacter(p);

		for (const Door& d : doors){
			d.draw(p);
		}
		player.draw(p);
		if (state == State::Event){
			eventManager.drawEvent();
		}
		else{
			drawItemInventory();
		}
		camera.debug();
		TextureAsset(L"texELMagicUpS").draw(Point(1055, 60));
		fn.draw(Format(L"× ", itemCount,L"/",(int)itemManager.getSize()), { 1100, 50 },Palette::White);
	}
private:
	void loadDoors(const String stageName){
		doors.clear();

		CSVReader csv(L"data/Elis/Map/" + nowMapName + L"/" +  stageName + L".csv");
		if (csv){
			for (size_t y = 0; y < csv.rows; ++y){
				if (csv.get<String>(y, 0) == L"door"){
					Point doorPos = { csv.get<int>(y, 1), csv.get<int>(y, 2) };
					String nextStage = csv.get<String>(y, 3);
					Point nextPos = { csv.get<int>(y, 4), csv.get<int>(y, 5) };
					doors.push_back(Door(doorPos, nextStage, nextPos));
				}
			}
		}
	}
	void drawItemInventory()const{
		for (size_t i = 0; i < 8; ++i){
			const Point p = { 100 + 70 * i, 50 };
			Rect(50, 50).setCenter(p).draw(Palette::White);
			if (i < havingItems.size()){
				TextureAsset(Item::GetItemAssetName(havingItems[i])).drawAt(p);
			}
		}
	}

	void intersectPlayertoMap(){
		moveX();
		jump();
		passFloor();
		fall();
		identify();
	}

	void passFloor(){
		const Point head = player.headCollisionPos();
		const Point foot = player.footCollisionPos();

		if (player.keyDown.clicked
			&& map.isFloor(foot.x, foot.y)
			&& map.getFloorType(foot.x, foot.y) == FloorType::PassFloor)//
		{
			player.moveChara({ 0, 1 });
		}

	}

	void moveX(){
		Point foot = player.footCollisionPos();
		Point head = player.headCollisionPos();

		const double vx = player.nextVec.x;
		const int dir = vx > 0 ? 1 : -1;
		for (int i = 0; i<std::abs(vx); ++i)
		{
			//riseOffset?...坂を上れるための調整値
			const int riseOffset = 3;
			for (int j = 0; j <= riseOffset; ++j)
			{
				const Point foot = player.footCollisionPos();
				const Point head = player.headCollisionPos();
				const Point fPos(foot.x + dir, foot.y - j - 1);
				const Point hPos(head.x + dir, head.y - j - 1);
				if (!(map.isFloor(fPos.x, fPos.y)) && !map.intersectBody(fPos, hPos, FloorType::NormalFloor))
				{
					player.moveChara(Point(dir, -j));
					break;
				}
			}
		}
	}
	void jump(){
		Player::JumpInfo& j = player.jumpInfo;

		if (player.keyUp.pressed && j.jumping)//ジャンプする
		{
			if (j.m_jumpH>0 && j.jumping)
			{
				//衝突検知
				for (int i = 1; i <= j.m_jumpH; ++i)
				{
					const Point foot = player.footCollisionPos();
					const Point head = player.headCollisionPos();
					const Point headPos(head.x, head.y - 1);
					const Point footPos(foot.x, foot.y - 1);

					if (map.isFloor(head.x, head.y - i))
					{
						if (map.getFloorType(headPos.x, headPos.y) == FloorType::NormalFloor)//床なら
						{
							player.jumpInfo.m_jumpH = i - 1;
							break;
						}
					}
				}
				player.moveChara({ 0, -player.jumpInfo.m_jumpH });
				player.jumpInfo.m_jumpH -= player.kDrop;
			}
		}
		else if (!player.keyUp.pressed && j.jumping)//
		{
			player.jumpInfo.m_jumpH = 0;
		}
	}
	void fall(){
		//落下に関する部分
		if (player.jumpInfo.jumping && player.jumpInfo.m_jumpH - player.m_gravity >0)
		{
			player.moveChara({ 0, player.m_gravity });
			return;
		}

		const Point foot = player.footCollisionPos();
		for (int i = 0; i <= player.m_gravity; ++i)
		{
			//衝突検知
			if (map.isFloor(foot.x, foot.y + i))
			{
				player.moveChara({ 0, i });
				return;
			}
		}
		player.moveChara({ 0, player.m_gravity });
	}

	void identify(){
		//identifyに関する部分
		const Point foot = player.footCollisionPos();
		const Point head = player.headCollisionPos();
		if (map.isFloor(foot.x, foot.y)== false || player.jumpInfo.m_jumpH - player.m_gravity >0)//
		{
			player.elisState = Player::ElisState::Jumping;
			player.m_gravity = Min(player.m_gravity + 1, player.kMaxGravity);
			return;
		}

		player.jumpInfo.jumping = false;
		player.m_gravity = player.kGravity0;
		player.jumpInfo.m_jumpH = 0;

		const double vx = player.nextVec.x;
		if (vx != 0)
		{
			if (player.elisState == Player::ElisState::Jumping){
				SoundAsset(L"sndElisLand").play();//////着地音
			}
			player.elisState = Player::ElisState::Running;
			return;
		}

		if (player.elisState == Player::ElisState::Jumping){
			SoundAsset(L"sndElisLand").play();//////着地音
		}
		player.elisState = Player::ElisState::Staying;
	}


	State state = State::Playing;

	String nowStageName;
	String nowMapName;

	Map map;
	Player player;
	MyCamera camera;
	EventManager eventManager;
	ItemManager itemManager;
	Font fn = Font(30, Typeface::Heavy);

	bool mapMoving = false;
	Array<Door> doors;
	Door nextDoor;

	Array<Item::Type> havingItems;
	int itemCount = 0;//集めた宝石の数
};