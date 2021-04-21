
//          _                          _          __ ___   ___   ___  
//    /\   | |                        | |        /_ |__ \ / _ \ / _ \ 
//   /  \  | | _____  ____ _ _ __   __| | ___ _ __| |  ) | | | | (_) |
//  / /\ \ | |/ _ \ \/ / _` | '_ \ / _` |/ _ \ '__| | / /| | | |> _ < 
// / ____ \| |  __/>  < (_| | | | | (_| |  __/ |  | |/ /_| |_| | (_) |
///_/    \_\_|\___/_/\_\__,_|_| |_|\__,_|\___|_|  |_|____|\___/ \___/ 
#pragma once
//#undef DrawText
#include "includes.hpp"
#include <string>
class Renderer
{
public:
	void Initialize();

	void BeginScene();
	void DrawScene();
	void EndScene();

	float DrawOutlinedText(ImFont* pFont, const std::string& text, const ImVec2& position, float size, uint32_t color, bool center);
	float DrawText(ImFont* pFont, const std::string& text, const ImVec2& position, float size, uint32_t color, bool center);
	void DrawLine(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness = 1.0f);
	void DrawCircle(const ImVec2& position, float radius, uint32_t color, float thickness = 1.0f);
	void DrawCircleScale(const ImVec2& position, float radius, uint32_t color, const ImVec2& scalepos, const ImVec2& scaleHeadPosition, float thickness = 1.0f);
	void DrawCircleFilled(const ImVec2& position, float radius, uint32_t color);
	void DrawBox(const ImVec2& position, const ImVec2& HeadPosition, uint32_t color);
	void DrawHealth(const ImVec2& scalepos, const ImVec2& scaleHeadPosition, INT8 health, float thickness = 1.0f);
	float RenderText(ImFont* m_pFont,const std::string& text, const ImVec2& position, float size, uint32_t color, bool center);
	void RenderImageRounded(ID3D11ShaderResourceView* pTexture, const ImVec2& from, const ImVec2& to, uint32_t color, float rounding, uint32_t roundingCornersFlags);
	void RenderImage(ID3D11ShaderResourceView* pTexture, const ImVec2& from, const ImVec2& to, uint32_t color);
	void RenderRectFilled(const ImVec2& from, const ImVec2& to, uint32_t color, float rounding, uint32_t roundingCornersFlags);
	void RenderRect(const ImVec2& from, const ImVec2& to, uint32_t color, float rounding, uint32_t roundingCornersFlags, float thickness);

	ImFont* m_pDefault;
	ImFont* m_pTabs;
	ImFont* m_pSubTabs;

	static Renderer* GetInstance();

private:
	Renderer();
	~Renderer();

	static Renderer* m_pInstance;
};