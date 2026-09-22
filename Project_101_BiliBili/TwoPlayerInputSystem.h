#pragma once

#include <array>
#include <cstddef>

#include "InputInfo.h"

enum class CONTROLLER_HALF
{
	LEFT,
	RIGHT
};

struct CharacterControlInput
{
	DirectX::XMFLOAT2 move{};
	InputState shoot{};
};

class TwoPlayerInputSystem
{
public:
	static constexpr std::size_t PLAYER_COUNT = 2;
	static constexpr std::size_t CHARACTERS_PER_PLAYER = 2;
	static constexpr std::size_t CHARACTER_COUNT = PLAYER_COUNT * CHARACTERS_PER_PLAYER;

	void Update(const InputInfo* pInputInfo);

	const CharacterControlInput* GetCharacterInput(std::size_t characterIndex) const;
	const CharacterControlInput* GetCharacterInput(
		std::size_t playerIndex,
		CONTROLLER_HALF controllerHalf
	) const;

private:
	std::array<CharacterControlInput, CHARACTER_COUNT> m_characterInputs{};
};
