#include "PokemonSoundRenderer.h"
#include "../Gameplay/Data/Settings.h"

const unsigned int PokemonSoundRenderer::SOUND_RENDERER_CHANNELS = 10;

PokemonSoundRenderer::PokemonSoundRenderer(PokemonGameEventDispatcher& ged) : 
	ska::Observer<SettingsChangeEvent>(std::bind(&PokemonSoundRenderer::onSettingsChange, this, std::placeholders::_1)),
	ska::SoundRenderer(SOUND_RENDERER_CHANNELS),
	m_ged(ged) {
	m_ged.ska::Observable<SettingsChangeEvent>::addObserver(*this);
}

PokemonSoundRenderer::~PokemonSoundRenderer() {
	m_ged.ska::Observable<SettingsChangeEvent>::removeObserver(*this);
}

bool PokemonSoundRenderer::onSettingsChange(SettingsChangeEvent& sce) {
	if ((sce.eventType & SettingsChangeEventType::MUSIC_VOLUME) == SettingsChangeEventType::MUSIC_VOLUME) {
		setMusicVolume(sce.settings.getSoundVolume());
	}
	return true;
}
