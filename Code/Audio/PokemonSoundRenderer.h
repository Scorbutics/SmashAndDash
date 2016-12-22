#pragma once

#include "../ska/Utils/Observer.h"
#include "../ska/Audio/SoundRenderer.h"
#include "../Gameplay/Data/SettingsChangeEvent.h"
#include "../Gameplay/PokemonGameEventDispatcher.h"

class PokemonSoundRenderer : 
	public ska::SoundRenderer,
	public ska::Observer<SettingsChangeEvent> {

public:
	PokemonSoundRenderer(PokemonGameEventDispatcher& ged);
	bool onSettingsChange(SettingsChangeEvent& sce);
	virtual ~PokemonSoundRenderer();

private:
	PokemonGameEventDispatcher& m_ged;
	static const unsigned int SOUND_RENDERER_CHANNELS;
};

