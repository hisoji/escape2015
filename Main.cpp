#include<Siv3D.hpp>

const Point SCREEN_SIZE(1280, 720);

enum class GameState{
	Loading,
	Title,
	Play,
	StageSelect,
	Num,
};

//GameStateを跨いで共有されるデータ群
struct GameData{
	GameData(){}
	GameState gameState = GameState::Loading;
	bool isRightHand = true;

	String mapName;
	String stageName = L"stage00";
};

GameData g_gameData;

void ChangeScene(GameState state){
	g_gameData.gameState = state;
}

double fadeOutTime(){
	return 1000;
}
void blackFade(const TimerMillisec &timer)
{
	const int alpha = Min(static_cast<int>(255*timer.elapsed() / fadeOutTime()), 255);
	Rect(0, 0, 1280, 720).draw(Color(0, 0, 0, alpha));
}

class Loading{
public:
	void init(){
		frame = 0;
	}

	void update(){
		if (frame == 10){
			load();
			ChangeScene(GameState::Play);
		}

		++frame;
	}
	void draw()const{
		fn.draw(L"Now loading...", { 1000, 600 });
	}
private:
	int frame = 0;
	Font fn = Font(20, Typeface::Thin);
	void load(){
		//Title
		TextureAsset::Register(L"texElisTitle", L"data/Elis/Title/title.png");
		TextureAsset::Register(L"texElisNewgame", L"data/Elis/Title/newgame.png");
		TextureAsset::Register(L"texElisContinue", L"data/Elis/Title/continue.png");
		TextureAsset::Register(L"texElisConfig", L"data/Elis/Title/config.png");
		SoundAsset::Register(L"sndElisTitle", L"data/Elis/BGM/title.mp3");
		SoundAsset::Register(L"sndElisPushstart", L"data/Elis/SE/System/pushstart.mp3");
		SoundAsset::Register(L"sndELsystemButton", L"data/Elis/SE/System/systemButton.mp3");

		//Title config
		TextureAsset::Register(L"texELconfigbase", L"data/Elis/Title/config/configbase.png");
		TextureAsset::Register(L"texELconfigkey", L"data/Elis/Title/config/key.png");
		TextureAsset::Register(L"texELconfig_select", L"data/Elis/Title/config/button_select.png");
		TextureAsset::Register(L"texELconfig_right", L"data/Elis/Title/config/button_right.png");
		TextureAsset::Register(L"texELconfig_left", L"data/Elis/Title/config/button_left.png");
		TextureAsset::Register(L"texELconfig_back", L"data/Elis/Title/config/button_back.png");


		//Stageselect
		TextureAsset::Register(L"texELcaveIcon", L"data/Elis/StageSelect/cave.png", { L"stageicon" });
		TextureAsset::Register(L"texELfortressIcon", L"data/Elis/StageSelect/fortress.png", { L"stageicon" });
		TextureAsset::Register(L"texELpalaceIcon", L"data/Elis/StageSelect/palace.png", { L"stageicon" });
		TextureAsset::Register(L"texELportIcon", L"data/Elis/StageSelect/port.png", { L"stageicon" });
		TextureAsset::Register(L"texELtempleIcon", L"data/Elis/StageSelect/temple.png", { L"stageicon" });
		TextureAsset::Register(L"texELtowerIcon", L"data/Elis/StageSelect/tower.png", { L"stageicon" });
		TextureAsset::Register(L"texELworldmap", L"data/Elis/StageSelect/worldmap.png", { L"stageicon" });
		TextureAsset::Register(L"texELmapbit", L"data/Elis/StageSelect/mapbit.png", { L"stageicon" });
		SoundAsset::Register(L"sndELstageselect", L"data/Elis/BGM/stageselect.mp3");
		SoundAsset::Register(L"sndELstage_choice", L"data/Elis/SE/System/stage_choice.mp3");

		const String signPath = L"data/Elis/StageSelect/sign/";
		TextureAsset::Register(L"texELcave_sign", signPath + L"cave_sign.png");
		TextureAsset::Register(L"texELport_sign", signPath + L"port_sign.png");
		TextureAsset::Register(L"texELtemple_sign", signPath + L"temple_sign.png");
		TextureAsset::Register(L"texELfortress_sign", signPath + L"fortress_sign.png");
		TextureAsset::Register(L"texELpalace_sign", signPath + L"palace_sign.png");
		TextureAsset::Register(L"texELtower_sign", signPath + L"tower_sign.png");

		//maingame
		const String dataPath = L"data/Elis/";
		const String playerPath = dataPath + L"Player/";

		TextureAsset::Register(L"texElis_readyCharging", playerPath + L"elis_readyCharging.png");
		TextureAsset::Register(L"texELStick_readyCharging", playerPath + L"stick_readyCharging.png");
		TextureAsset::Register(L"texElis_charge", playerPath + L"elis_charge.png");
		TextureAsset::Register(L"texELStick_charge", playerPath + L"stick_charge.png");
		TextureAsset::Register(L"texElis_swing", playerPath + L"elis_swing.png");
		TextureAsset::Register(L"texELStick_swing", playerPath + L"stick_swing.png");
		TextureAsset::Register(L"texElis_damaged", playerPath + L"elis_damaged.png");
		TextureAsset::Register(L"texElis_dead", playerPath + L"elis_dead.png");
		TextureAsset::Register(L"ELcharge1", playerPath + L"charge1.png");
		TextureAsset::Register(L"ELcharge2", playerPath + L"charge2.png");
		TextureAsset::Register(L"ELcharge3", playerPath + L"charge3.png");
		TextureAsset::Register(L"ELBcharge1", playerPath + L"Bcharge1.png");
		TextureAsset::Register(L"ELBcharge2", playerPath + L"Bcharge2.png");
		TextureAsset::Register(L"ELBcharge3", playerPath + L"Bcharge3.png");
		TextureAsset::Register(L"ELScharge1", playerPath + L"Scharge1.png");
		TextureAsset::Register(L"ELScharge2", playerPath + L"Scharge2.png");
		TextureAsset::Register(L"ELScharge3", playerPath + L"Scharge3.png");
		TextureAsset::Register(L"ELRcharge1", playerPath + L"Rcharge1.png");
		TextureAsset::Register(L"ELRcharge2", playerPath + L"Rcharge2.png");
		TextureAsset::Register(L"ELRcharge3", playerPath + L"Rcharge3.png");
		TextureAsset::Register(L"ELGcharge1", playerPath + L"Gcharge1.png");
		TextureAsset::Register(L"ELGcharge2", playerPath + L"Gcharge2.png");
		TextureAsset::Register(L"ELGcharge3", playerPath + L"Gcharge3.png");

		const String elisSEPath = dataPath + L"SE/Elis/";

		SoundAsset::Register(L"sndElisJump", elisSEPath + L"ElisJump.mp3");//跳躍音
		SoundAsset::Register(L"sndElisLand", elisSEPath + L"ElisLand.mp3");//着地音
		SoundAsset::Register(L"sndElisCharge", elisSEPath + L"ElisCharge.mp3");//溜め中
		SoundAsset::Register(L"sndElisDead", elisSEPath + L"ElisDead.mp3");//死亡中
		SoundAsset::Register(L"sndElisDamaged", elisSEPath + L"ElisDamaged.mp3");//くらった
		SoundAsset::Register(L"sndElisIce", elisSEPath + L"ElisIce.mp3");//氷魔法
		SoundAsset::Register(L"sndELGetItem", dataPath + L"SE/System/getItem.mp3");
		SoundAsset::Register(L"sndELMagicShoot", elisSEPath + L"ElisMagicShoot.mp3");
		SoundAsset::Register(L"sndElisStone", elisSEPath + L"ElisStone.mp3");
		SoundAsset::Register(L"sndElisFire", elisSEPath + L"ElisFire.mp3");
		SoundAsset::Register(L"sndElisWind", elisSEPath + L"ElisWind.mp3");

		TextureAsset::Register(L"texElisNormal", playerPath + L"elis_stay.png");
		TextureAsset::Register(L"texElisRun", playerPath + L"elis_move.png");
		TextureAsset::Register(L"texElisJump", playerPath + L"elis_jump.png");
		TextureAsset::Register(L"texElisFire", playerPath + L"elis_fire.png");

		//map
		SoundAsset::Register(L"ELSEdoor", L"data/Elis/SE/System/door.mp3");
		SoundAsset::Register(L"ELBGMtower", L"data/Elis/BGM/tower.mp3");
		SoundAsset::Register(L"ELBGMpalace", L"data/Elis/BGM/palace.mp3");
		SoundAsset::Register(L"ELBGMfortress", L"data/Elis/BGM/fortress.mp3");
		SoundAsset::Register(L"ELBGMtemple", L"data/Elis/BGM/temple.mp3");
		SoundAsset::Register(L"ELBGMfreezedport", L"data/Elis/BGM/freezedport.mp3");
		SoundAsset::Register(L"ELBGMlastboss1", L"data/Elis/BGM/lastboss1.mp3");
		SoundAsset::Register(L"ELBGMtutorial", L"data/Elis/BGM/tutorial.mp3");
		SoundAsset::Register(L"ELBGMbossbattle", L"data/Elis/BGM/bossbattle.mp3");


		TextureAsset::Register(L"ELlogotower", L"data/Elis/Map/logo/tower.png");
		TextureAsset::Register(L"ELlogocave", L"data/Elis/Map/logo/cave.png");
		TextureAsset::Register(L"ELlogotemple", L"data/Elis/Map/logo/temple.png");
		TextureAsset::Register(L"ELlogopalace", L"data/Elis/Map/logo/palace.png");
		TextureAsset::Register(L"ELlogoport", L"data/Elis/Map/logo/port.png");
		TextureAsset::Register(L"ELlogofortress", L"data/Elis/Map/logo/fortress.png");

		TextureAsset::Register(L"ELlogoclear", L"data/Elis/Map/logo/clear.png");
		TextureAsset::Register(L"ELlogobossbattle", L"data/Elis/Map/logoBossbattle.png");

		SoundAsset::Register(L"ELSEstage_start", L"data/Elis/SE/System/stage_start.mp3");

		SoundAsset::Register(L"ELSEbossbattle_start", L"data/Elis/SE/System/bossbattle_start.mp3");

	}
};


class Title{
	enum class State{
		Main,
		Config,
	};

public:
	Title(){
		kButtonRect = Rect(0, 0, 178, 40);
		kConfigPos = Point(355, 29);
		kConfSelectRect = Rect(Point(450, 594) + kConfigPos, 76, 37);
		kConfRightRect = Rect(Point(323, 105) + kConfigPos, 138, 46);
		kConfLeftRect = Rect(Point(110, 105) + kConfigPos, 138, 46);
	}
	void init(){
		onPut = false;
		decided = false;
		m_state = State::Main;
		fadeOutTimer.reset();
		SoundAsset(L"sndElisTitle").play();
	}

	void update(){
		switch (m_state){
		case State::Main:
			updateTitle();
			break;
		case State::Config:
			updateConfig();
			break;
		default:
			break;
		}
	}

	void draw()const{
		TextureAsset(L"texElisTitle").draw();

		switch (m_state){
		case State::Main:
			drawTitle();
			break;
		case State::Config:
			drawConfig();
			break;
		default:
			break;
		}
	}

private:
	void updateOnPut(const Array<Rect>& rects){
		const Point p = Mouse::Pos();
		bool intersect = false;
		for (Rect r : rects){
			if (p.intersects(r)){
				intersect = true;
				if (!onPut){
					SoundAsset(L"sndELsystemButton").playMulti();
					onPut = true;
				}
			}
		}
		if (intersect == false){
			onPut = false;
		}
	}

	void updateTitle(){


		if (decided){
			if (fadeOutTimer.elapsed() > fadeOutTime()){
				SoundAsset(L"sndElisTitle").stop();
				ChangeScene(GameState::StageSelect);
			}
		}
		else{
			const Array<Rect> rects = {
				Rect(kButtonRect.pos + Point(640 - 89, 410 - 20), kButtonRect.size),//newgame
				Rect(kButtonRect.pos + Point(640 - 89, 480 - 20), kButtonRect.size),//continue
				Rect(kButtonRect.pos + Point(640 - 89, 550 - 20), kButtonRect.size)//config
			};
			updateOnPut(rects);

			const Point p = Mouse::Pos();
			if (p.intersects(rects[0]) && Mouse::LeftClicked())
			{
				SoundAsset(L"sndElisPushstart").play();
				fadeOutTimer.start();
				decided = true;
				onPut = false;
			}
			else if (p.intersects(rects[1]) && Mouse::LeftClicked())
			{
				SoundAsset(L"sndElisPushstart").play();
				fadeOutTimer.start();
				decided = true;
				onPut = false;
			}
			else if (p.intersects(rects[2]) && Mouse::LeftClicked())
			{
				SoundAsset(L"sndElisPushstart").play();
				m_state = State::Config;
				onPut = false;
			}
		}
	}

	void updateConfig(){
		const Point p = Mouse::Pos();

		const Array<Rect> rects = {
			kConfSelectRect,
			kConfLeftRect,
			kConfRightRect
		};
		updateOnPut(rects);

		if (p.intersects(rects[0]) && Mouse::LeftClicked())
		{
			SoundAsset(L"sndElisPushstart").play();
			m_state = State::Main;
			onPut = false;
		}
		else if (p.intersects(rects[1]) && Mouse::LeftClicked())
		{
			SoundAsset(L"sndElisPushstart").play();
			g_gameData.isRightHand = false;
			onPut = false;
		}
		else if (p.intersects(rects[2]) && Mouse::LeftClicked())
		{
			SoundAsset(L"sndElisPushstart").play();
			g_gameData.isRightHand = true;
			onPut = false;
		}
	}
	void drawTitle()const{
		const Point p = Mouse::Pos();

		const Array<String> assets = {
			L"texElisNewgame",
			L"texElisContinue",
			L"texElisConfig"
		};
		for (int i = 0; i < 3; ++i){
			TextureAsset(assets[i])(0, 40 * p.intersects(Rect(kButtonRect.pos + Point(640 - 89, 410 + 70 * i - 20), kButtonRect.size)), 178, 40).drawAt(640, 410 + 70 * i);
		}

		if (decided){
			blackFade(fadeOutTimer);
		}
	}
	void drawConfig()const{
		TextureAsset(L"texELconfigbase").draw(kConfigPos);

		const Point p = Mouse::Pos();

		const Rect r = g_gameData.isRightHand ? kConfRightRect : kConfLeftRect;
		r.draw(Color(255, 255, 255, 155));

		TextureAsset(L"texELconfigkey")(0, 299 * g_gameData.isRightHand, 571, 299).draw(Point(0, 170) + kConfigPos);

		TextureAsset(L"texELconfig_right")(0, 46 * p.intersects(kConfRightRect), 138, 46).draw(kConfRightRect.pos);
		TextureAsset(L"texELconfig_left")(0, 46 * p.intersects(kConfLeftRect), 138, 46).draw(kConfLeftRect.pos);

		TextureAsset(L"texELconfig_select")(0, 37 * p.intersects(kConfSelectRect)
			, kConfSelectRect.size.x, kConfSelectRect.size.y).draw(kConfSelectRect.pos);
	}

	bool decided = false;
	State m_state;
	TimerMillisec fadeOutTimer;
	bool onPut = false;

	Rect kButtonRect;
	Point kConfigPos;
	Rect kConfSelectRect;
	Rect kConfRightRect;
	Rect kConfLeftRect;
};
class StageSelect{
public:
	struct StageInfo{
		String iconAsset;
		String signAsset;
		Circle circle;
		Vec2 iconPos;
		Point signPos;
		int frame;
	};
	Array<StageInfo> stages;

	StageSelect(){
		const int kR = 130;
		const Circle kCirCave(46 + 291.0 / 2, 91 + 226 / 2, kR);
		const Circle kCirPort(438 + 352 / 2, 137 + 135.0 / 2, kR);
		const Circle kCirTemple(186 + 292 / 2, 329 + 187.0 / 2, kR);
		const Circle kCirFortress(486 + 299.0 / 2, 496 + 207.0 / 2, kR);
		const Circle kCirPalace(844 + 344 / 2, 402 + 182 / 2, kR);
		const Circle kCirTower(828 + 423.0 / 2, 26 + 293.0 / 2, kR);

		stages.push_back({ L"texELcaveIcon", L"texELcave_sign", kCirCave, { 46, 91 }, { 317, 208 } });
		stages.push_back({ L"texELportIcon", L"texELport_sign", kCirPort, { 438, 137 }, { 744, 206 } });
		stages.push_back({ L"texELtempleIcon", L"texELtemple_sign", kCirTemple, { 186, 329 }, { 444, 397 } });
		stages.push_back({ L"texELfortressIcon", L"texELfortress_sign", kCirFortress, { 486, 496 }, { 773, 576 } });
		stages.push_back({ L"texELpalaceIcon", L"texELpalace_sign", kCirPalace, { 844, 402 }, { 844, 402 } });
		stages.push_back({ L"texELtowerIcon", L"texELtower_sign", kCirTower, { 828, 26 }, { 1140, 198 } });
	}

	void init(){
		fadeOutTimer.reset();
		decided = false;
		onPut = false;
		SoundAsset(L"sndELstageselect").play();
	}
	void update(){
		if (decided){
			if (fadeOutTimer.elapsed() > fadeOutTime()){
				SoundAsset(L"sndELstageselect").stop();
				TextureAsset::ReleaseByTag(L"stageicon");
				ChangeScene(GameState::Play);
			}
		}
		else{
			if (Mouse::LeftClicked()){
				SoundAsset(L"sndElisPushstart").play();
				decided = true;
				fadeOutTimer.start();
			}

			bool intersect = false;
			for (StageInfo& s : stages){
				if (s.circle.intersects(Mouse::Pos())){
					intersect = true;

					const int animeframe = 4;
					const int maxframe = animeframe * 2;
					s.frame = Min(++s.frame, maxframe);

					if (onPut == false){
						SoundAsset(L"sndELsystemButton").playMulti();
						onPut = true;
					}
				}
				else{
					s.frame = 0;
				}
			}
			if (intersect == false){
				onPut = false;
			}
		}
	}

	void draw()const{
		TextureAsset(L"texELworldmap").draw();

		drawBitRoad({ 342 + 25, 284 - 14 }, { 28, -8 }, 0, 2);//toPort
		drawBitRoad({ 595, 270 }, { -17, 29 }, 1, 6);//toFortress
		drawBitRoad({ 315, 518 }, { 24, 22 }, 1, 6);//toTemple
		drawBitRoad({ 785, 669 }, { 32, -14 }, 1, 5);//toPalace
		drawBitRoad({ 1016 + 5, 506 - 20 }, { 2, -30 }, 3, 5);//toTower

		for (const StageInfo& s : stages){
			showGeneralAnime(s);
		}

		if (decided){
			blackFade(fadeOutTimer);
		}
	}

private:

	void drawIcon(const String& assetName, const Vec2& drawPos, bool intersect = false)const{
		const TextureAsset& t = TextureAsset(assetName);
		t(0, intersect ? t.height / 2 : 0, t.width, t.height / 2).draw(drawPos);
	}

	void drawSign(const String& assetName, const Point& pos, int frameCount, int maxFrame)const
	{
		TextureAsset(assetName)(0, 126, 132, 126).draw
			(pos.x + ((frameCount == maxFrame) ? 0 : ((frameCount % 4 >= 2) ? frameCount / 2 : -frameCount / 2))
			, pos.y);
	}

	void showGeneralAnime(const StageInfo& s)const{
		bool intersect = s.circle.intersects(Mouse::Pos());
		bool cleared = true;
		if (intersect)
		{
			if (intersect && cleared){
				s.circle.draw(Color(255, 255, 255, 200));
			}
			if (cleared){
				drawIcon(s.iconAsset, s.iconPos, intersect);
			}
			const int animeframe = 4;
			const int maxframe = animeframe * 2;
			drawSign(s.signAsset, s.signPos, s.frame, maxframe);
		}
		else
		{
			if (cleared){
				drawIcon(s.iconAsset, s.iconPos, intersect);
			}
		}
	}
	void drawBitRoad(Point startPos, Point interval, int start, int end)const{
		const TextureAsset& t = TextureAsset(L"texELmapbit");
		for (int i = start; i <= end; ++i){
			t.drawAt(startPos + interval * i);
		}
	}
	bool decided = false;
	Font fn = Font(30, Typeface::Bold);
	bool onPut = false;
	TimerMillisec fadeOutTimer;
};

#include"MainGame.h"





class Elis
{
public:
	Elis(){}

	bool init(){
		Window::Resize({ 1280, 720 });
		Window::SetTitle(L" Elis");

		initState(g_gameData.gameState);
		nowState = g_gameData.gameState;
		return true;
	}

	void update(){
		if (nowState != g_gameData.gameState){
			initState(g_gameData.gameState);
			nowState = g_gameData.gameState;
		}

		switch (nowState)
		{
		case GameState::Loading:
			m_loading.update();
			break;
		case GameState::Title:
			m_title.update();
			break;
		case GameState::Play:
			m_mainGame.update();
			break;
		case GameState::StageSelect:
			m_stageSelect.update();
			break;
		default:
			break;
		}
	}
	void draw()const{
		switch (nowState){
		case GameState::Loading:
			m_loading.draw();
			break;
		case GameState::Title:
			m_title.draw();
			break;
		case GameState::Play:
			m_mainGame.draw();
			break;
		case GameState::StageSelect:
			m_stageSelect.draw();
			break;
		default:
			break;
		}
	}

	void initState(GameState nextState){
		switch (nextState)
		{
		case GameState::Loading:
			m_loading.init();
			break;
		case GameState::Title:
			m_title.init();
			break;
		case GameState::Play:
			m_mainGame.init();
			break;
		case GameState::StageSelect:
			m_stageSelect.init();
			break;
		default:
			break;
		}
	}

private:
	GameState nowState;
	Loading m_loading;
	Title m_title;
	StageSelect m_stageSelect;
	MainGame m_mainGame;
};

void Main()
{
	Elis elis;

	if (!elis.init()){
		return;
	}

	while (System::Update())
	{
		elis.update();

		elis.draw();
	}
}