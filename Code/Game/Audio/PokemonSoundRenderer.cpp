#include "PokemonSoundRenderer.h"
#include "../Gameplay/Data/Settings.h"
#include "../Gameplay/PokemonGameEventDispatcherDeclaration.h"

const unsigned int PokemonSoundRenderer::SOUND_RENDERER_CHANNELS = 10;

PokemonSoundRenderer::PokemonSoundRenderer(PokemonGameEventDispatcher& ged) :
    SoundRenderer(SOUND_RENDERER_CHANNELS),
	ska::Observer<SettingsChangeEvent>(bind(&PokemonSoundRenderer::onSettingsChange, this, std::placeholders::_1)),
	m_ged(ged){
	m_ged.ska::Observable<SettingsChangeEvent>::addObserver(*this);
}

PokemonSoundRenderer::~PokemonSoundRenderer() {
	m_ged.ska::Observable<SettingsChangeEvent>::removeObserver(*this);
}

bool PokemonSoundRenderer::onSettingsChange(SettingsChangeEvent& sce) {
	if ((sce.eventType & MUSIC_VOLUME) == MUSIC_VOLUME) {
		setMusicVolume(sce.settings.getSoundVolume());
	}
	return true;
}
