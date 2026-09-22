#include "TwoPlayerInputSystem.h"

void TwoPlayerInputSystem::Update(const InputInfo* pInputInfo)
{
	m_characterInputs = {};
	if (!pInputInfo)
	{
		return;
	}

	for (std::size_t playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex)
	{
		const auto& controller = pInputInfo->controller[playerIndex];
		auto& leftCharacter = m_characterInputs[playerIndex * CHARACTERS_PER_PLAYER];
		auto& rightCharacter = m_characterInputs[playerIndex * CHARACTERS_PER_PLAYER + 1];

		leftCharacter.move = controller.leftStick;
		leftCharacter.shoot = controller.LSHOULDER;

		rightCharacter.move = controller.rightStick;
		rightCharacter.shoot = controller.RSHOULDER;
	}
}

const CharacterControlInput* TwoPlayerInputSystem::GetCharacterInput(
	std::size_t characterIndex
) const
{
	if (characterIndex >= CHARACTER_COUNT)
	{
		return nullptr;
	}

	return &m_characterInputs[characterIndex];
}

const CharacterControlInput* TwoPlayerInputSystem::GetCharacterInput(
	std::size_t playerIndex,
	CONTROLLER_HALF controllerHalf
) const
{
	if (playerIndex >= PLAYER_COUNT)
	{
		return nullptr;
	}

	const std::size_t halfOffset =
		controllerHalf == CONTROLLER_HALF::LEFT ? 0 : 1;

	return GetCharacterInput(playerIndex * CHARACTERS_PER_PLAYER + halfOffset);
}
