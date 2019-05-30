#pragma once
#include "Game/__internalConfig/LoggerConfig.h"
#include "Base/Patterns/Observer.h"
#include "Audio/SoundRenderer.h"
#include "../Gameplay/Data/SettingsChangeEvent.h"
#include "../Gameplay/PokemonGameEventDispatcher.h"

class PokemonSoundRenderer :
	public ska::SoundRenderer,
	public ska::Observer<SettingsChangeEvent> {

public:
	explicit PokemonSoundRenderer(PokemonGameEventDispatcher& ged);
	PokemonSoundRenderer(PokemonSoundRenderer&&) = default;
	PokemonSoundRenderer(const PokemonSoundRenderer&) = delete;
	void operator=(const PokemonSoundRenderer&) = delete;

	bool onSettingsChange(SettingsChangeEvent& sce);
	virtual ~PokemonSoundRenderer();

private:
	PokemonGameEventDispatcher& m_ged;
	static const unsigned int SOUND_RENDERER_CHANNELS;
};

