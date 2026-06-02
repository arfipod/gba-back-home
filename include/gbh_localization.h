#ifndef GBH_LOCALIZATION_H
#define GBH_LOCALIZATION_H

#include <cstdint>

namespace gbh
{
    enum class Language : uint8_t
    {
        spanish,
        english
    };

    enum class TextId : uint8_t
    {
        seek_threshold,
        title_subtitle,
        title_genre,
        title_start,
        title_peace_seed,
        title_language,
        pilgrim_title,
        pilgrim_change,
        pilgrim_confirm,
        lore_title,
        action_enter,
        action_title,
        lore_controls,
        hud_dwelling_label,
        hud_key_marker,
        hud_mercy_label,
        help_controls,
        pause,
        dungeon_regenerated,
        resume_path,
        silence_threshold_hint,
        wall_teaches,
        missing_key,
        shrine_prompt,
        silence_heavy,
        bread,
        candle,
        inner_key,
        resolution,
        strike_no_discern,
        no_light,
        mercy_light,
        light_discerns,
        threshold_yields,
        shrine_restored,
        remain,
        silence_light,
        noise_near,
        call_east,
        call_west,
        call_south,
        call_north,
        call_here,
        no_memory,
        false_door_hurts,
        hope_rises,
        beast_hits,
        shadow_hurts,
        noise_drains,
        boss_intro,
        boss_title,
        boss_seals_label,
        boss_silence_window,
        boss_noise_window,
        boss_controls,
        true_silence,
        still_noise,
        door_empties,
        prayer_hurts,
        hit_feeds,
        noise_bites,
        boss_memory_good,
        boss_memory_low,
        victory_title,
        victory_restart,
        defeat_title,
        defeat_fall,
        defeat_retry,
        defeat_hint_no_light,
        defeat_hint_haste,
        defeat_hint_shadow
    };

    struct LocalizedText
    {
        const char* es;
        const char* en;
    };

    extern const LocalizedText text_table[];
    extern const LocalizedText pilgrim_names[];
    extern const LocalizedText pilgrim_rules[];
    extern const LocalizedText dwelling_lore[];
    extern const LocalizedText dwelling_rules[];
    extern const LocalizedText memory_texts[];

    const char* localize(Language language, const LocalizedText& value);
    const char* text(Language language, TextId id);
}

#endif
