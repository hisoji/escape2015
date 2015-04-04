#pragma once
#include<Siv3D.hpp>

class MyCamera
{
public:
	MyCamera(){}

	void update(const Point& playerPos, const Point& mapSize){
		Point pos = -Point((1280 - 128) / 2, 720 / 5 * 3) + playerPos;/*�v���C���[�ʒu����J����������������*/

		//�ȉ��J�����̈ʒu����(����?)
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
		loadStage(mapName, stageName);
		//loadStage(L"yume", L"stage00");
	}
	void update(){}
	void draw(const Point& offset = { 0, 0 })const{
		m_texMap(offset.x, offset.y, SCREEN_SIZE.x, SCREEN_SIZE.y).draw();
	}

	//���C������߂鎞�̏���
	void updateReturn(){
		SoundAsset(m_bgmAsset).stop();
	}

	void loadStage(const String& mapName, const String& stageName){
		m_imageTerrain = Image(L"data/Elis/Map/" + mapName + L"/" + stageName + L"_terrain.png");
		//m_texMap = Texture(L"data/Elis/Map/" + mapName + L"/" + stageName + L"_map.png");
		m_texMap = Texture(L"data/Elis/Map/" + mapName + L"/" + stageName + L"_terrain.png");
		m_bgmAsset = L"ELBGMtower";

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

	bool intersectBody(const Point& foot, const Point& head,const FloorType& floorType)const//�ʏ폰�Ɛg�̂̐ڐG����
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
	Image m_imageTerrain; //�n�`�Փ˗p�̉摜
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

	Point nextVec;//���ɓ����x�N�g���Bmaingame�Ŏg���Ă�

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
	const int kMaxJumpH = 27;//�W�����v�����x
	const int kDrop = 1;//�W�����v���x�̌����x���d�͉����x
	const int kGravity0 = 5;


private:
	void move(){
		ElisState next = ElisState::Staying;

		bool input = false;
		if (!(keyRight.pressed && keyLeft.pressed))//�����������
		{
			if (keyRight.pressed)///////////////�E�ɐi�ރL�[
			{
				input = true;
				face = Face::Right;
				m_speed = Min(m_speed + 1, kMaxSpeed);
				next = ElisState::Running;
			}
			if (keyLeft.pressed)////////////////���ɐi�ރL�[
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
		Scissor,
	};
	static String GetItemAssetName(Item::Type type){
		switch (type){
		case Item::Type::Key:
			return L"texELMagicUpM";
		default:
			return L"texELMagicUpS";
		}
	}

	Item(Type t,Point pos){
		type = t;
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
	bool erased;//��������true
};

class ItemManager{
public:
	Array<Item> items;

	void loadItem(const String& mapName, const String& stageName){
		items.clear();

		items.push_back({ Item::Type::Key, { 600, 600 } });
		items.push_back({ Item::Type::Scissor, { 800, 600 } });
	}
	void update(){
		Erase_if(items, [](const Item& i){
			return i.erased;
		});
	}

	Optional<Item::Type> findItem(const Rect& playerRect = { 0, 0, 0, 0 }){
		for (Item& i : items){
			if (i.rect.intersects(playerRect)){
				i.erased = true;
				return i.type;
			}
		}
		return none;
	}

	void draw(const Point& offset = { 0, 0 })const{	
		for (const Item &item : items){
			if (isInCamera(item.rect.center, offset))
			{
				const Point p = item.rect.center - offset;
				TextureAsset(item.assetName).drawAt(p);
			}
		}
	}

private:
	bool isInCamera(const Point& pos, const Point& camerapos)const
	{
		const int kXoffset = 300;
		const int kYoffset = 300;

		return pos.x > camerapos.x - kXoffset
			&& pos.x < camerapos.x + 1280 + kXoffset
			&& pos.y > camerapos.y - kYoffset
			&& pos.y <camerapos.y + 720 + kYoffset;
	}
};


struct Door{
public:
	Door(Point _pos,Item::Type type)
		:hitRect(_pos,50,80),openType(type){}
	void draw(const Point& p)const{
		hitRect.movedBy(-p).draw(Palette::Blue);
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
	Item::Type openType;
};

enum EventType
{
	TalkEvent,
	AutoEvent,
	Num,
};

struct EventInfo
{
	Rect eventArea;
	String filename;
	EventType type;
	bool readed = false;//�����C�x���g���A���ŕ\�������̂�h�����߂̉��}���u
};

class EventManager
{
private:
	std::vector< EventInfo > m_events;
	bool m_isEventEnd = false;
	Font m_TalkFont = Font(30, Typeface::Default);
	int m_eventIndex = 0;
public:
	EventManager(){}

	//�}�b�v���̃C�x���g�����[�h����B�}�b�v�ɓ������u�ԂɌĂԁB
	void loadEvent(String stageName)
	{
		/*
		//�@data/Elis/event/mapname/stageName.csv
		CSVReader csv(L"data/Elis/Event/cave/stage00.csv");

		for (int y=0;y = csv.rows;++y)
		{
		EventInfo item;

		item.filename = csv.get<String>(y, 0).lower();

		}
		*/

		//��b�C�x���g�̎���
		EventInfo item1;
		item1.eventArea = Rect(832 - 50, 686 - 100, 100, 100);
		item1.filename = L"test";
		item1.type = EventType::TalkEvent;
		m_events.push_back(item1);

		//�����C�x���g�̎���
		EventInfo item2;
		item2.eventArea = Rect(1032 - 50, 686 - 100, 100, 100);
		item2.filename = L"unko";
		item2.type = EventType::AutoEvent;
		m_events.push_back(item2);
	}

	//�C�x���g����(������ǉ����Ă����B�l���������...�v���C���[�ʒu�A��b�{�^���������ꂽ��)
	void update()
	{
		if (Input::KeyZ.clicked)
		{
			m_isEventEnd = true;
		}
	}

	//�C�x���g�J�n
	void startEvent()
	{
		m_isEventEnd = false;
	}

	//��������C�x���g�����邩�Btrue�Ȃ�C�x���g���[�h�Ɉڂ�
	bool hasEvent(const Rect& playerRect, bool isClickedTalkButton)
	{
		bool flag = false;

		for (size_t i = 0; i < m_events.size(); ++i)
		{
			if (m_events[i].eventArea.intersects(playerRect))
			{
				if (m_events[i].type == EventType::TalkEvent
					&& isClickedTalkButton
					)
				{
					flag = true;
					m_eventIndex = i;
					break;
				}
				else if (m_events[i].type == EventType::AutoEvent
					&& !m_events[i].readed)
				{
					flag = true;
					m_eventIndex = i;
					m_events[i].readed = true;
					break;
				}
			}
			else if (m_events[i].type == EventType::AutoEvent)//�d�Ȃ��Ă���Ƃ��A���ŏo�Ȃ��悤�ɂ��邽�߂̏��u
			{
				m_events[i].readed = false;
			}
		}
		return flag;//false;//�C�x���g������������true,�����łȂ��Ȃ�false
	}

	//�C�x���g���I��������Btrue�Ȃ�ʏ�̃��[�h�Ɉڂ�
	bool isEnd(){
		return m_isEventEnd;//event�I���Ȃ�true��Ԃ�
	}

	void drawCharacter(const Point& cameraOffset) const
	{
		for (size_t i = 0; i < m_events.size(); ++i)
		{
			Rect(m_events[i].eventArea.pos - cameraOffset, m_events[i].eventArea.size).draw(Palette::Blue);
		}
	}

	void drawEvent() const
	{
		Rect(50, 50, 1180, 300).draw();
		m_TalkFont(m_events[m_eventIndex].filename).draw(100, 100, Palette::Black);
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
		player.init({ 100, 1000 });
		map.init(g_gameData.mapName,g_gameData.stageName);
		itemManager.loadItem(g_gameData.mapName, g_gameData.stageName);
		eventManager.loadEvent(g_gameData.stageName);

		doors.push_back(Door({ 800, 800 }, Item::Type::Key));
	}
	void update(){
		switch (state){
		case State::Playing:
			player.update();
			if (Input::KeyEnter.clicked){
				map.updateReturn();
				ChangeScene(GameState::StageSelect);
			}
			intersectPlayertoMap();
			camera.update(player.getPlayerPos(), map.getMapSize());

			itemManager.update();
			if (auto p = itemManager.findItem(player.elisRect)){
				havingItems.push_back(p.value());
			}

			//���������C�x���g���N����Ȃ�C�x���g�J��
			if (eventManager.hasEvent(player.getRect(),Input::KeyZ.clicked)){
				state = State::Event;
				eventManager.startEvent();
			}
			break;

		case State::Event:
			eventManager.update();

			//�C�x���g���I�������ʏ�̏�Ԃɖ߂�
			if (eventManager.isEnd()){
				state = State::Playing;
			}
			break;
		}
	}
	void draw()const{
		fn.drawCenter(L"�킦\n\n[Enter]�߂�", SCREEN_SIZE / 2);
		const Point& p = camera.getCameraPos();
		map.draw(p);
		player.draw(p);
		itemManager.draw(p);
		eventManager.drawCharacter(p);
		if (state == State::Event){
			eventManager.drawEvent();
		}
		
		/*
		for (const Door& d : doors){
			d.draw(p);
		}
		*/

		drawItemInventory();
	}
private:
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
			//riseOffset?...������邽�߂̒����l
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

		if (player.keyUp.pressed && j.jumping)//�W�����v����
		{
			if (j.m_jumpH>0 && j.jumping)
			{
				//�Փˌ��m
				for (int i = 1; i <= j.m_jumpH; ++i)
				{
					const Point foot = player.footCollisionPos();
					const Point head = player.headCollisionPos();
					const Point headPos(head.x, head.y - 1);
					const Point footPos(foot.x, foot.y - 1);

					if (map.isFloor(head.x, head.y - i))
					{
						if (map.getFloorType(headPos.x, headPos.y) == FloorType::NormalFloor)//���Ȃ�
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
		//�����Ɋւ��镔��
		if (player.jumpInfo.jumping && player.jumpInfo.m_jumpH - player.m_gravity >0)
		{
			player.moveChara({ 0, player.m_gravity });
			return;
		}

		const Point foot = player.footCollisionPos();
		for (int i = 0; i <= player.m_gravity; ++i)
		{
			//�Փˌ��m
			if (map.isFloor(foot.x, foot.y + i))
			{
				player.moveChara({ 0, i });
				return;
			}
		}
		player.moveChara({ 0, player.m_gravity });
	}

	void identify(){
		//identify�Ɋւ��镔��
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
				SoundAsset(L"sndElisLand").play();//////���n��
			}
			player.elisState = Player::ElisState::Running;
			return;
		}

		if (player.elisState == Player::ElisState::Jumping){
			SoundAsset(L"sndElisLand").play();//////���n��
		}
		player.elisState = Player::ElisState::Staying;
	}


	State state = State::Playing;

	Map map;
	Player player;
	MyCamera camera;
	EventManager eventManager;
	ItemManager itemManager;
	Font fn = Font(30, Typeface::Heavy);

	Array<Door> doors;

	Array<Item::Type> havingItems;
};