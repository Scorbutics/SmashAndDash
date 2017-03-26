#pragma once

#include "Utils/Observer.h"
#include "Audio/SoundRenderer.h"
#include "../Gameplay/Data/SettingsChangeEvent.h"
#include "../Gameplay/PokemonGameEventDispatcher.h"
#include "../Gameplay/PokemonGameEventDispatcherDeclaration.h"

class PokemonSoundRenderer :
	public ska::SoundRenderer,
	public ska::Observer<SettingsChangeEvent> {

public:
	explicit PokemonSoundRenderer(PokemonGameEventDispatcher& ged);
	void operator=(const PokemonSoundRenderer&) = delete;
	bool onSettingsChange(SettingsChangeEvent& sce);
	virtual ~PokemonSoundRenderer();

private:
	PokemonGameEventDispatcher& m_ged;
	static const unsigned int SOUND_RENDERER_CHANNELS;
};

