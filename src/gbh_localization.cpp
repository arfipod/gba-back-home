#include "gbh_localization.h"

namespace gbh
{
    const LocalizedText text_table[] = {
        { "Busca el umbral.", "Seek the threshold." },
        { "Regreso a la casa", "Return to the house" },
        { "Roguelike interior", "Interior roguelike" },
        { "A: peregrinar", "A: begin pilgrimage" },
        { "B: semilla paz", "B: peace seed" },
        { "SELECT: English", "SELECT: ESPANOL" },
        { "NOMBRE DEL PEREGRINO", "PILGRIM NAME" },
        { "LEFT/RIGHT cambia", "LEFT/RIGHT change" },
        { "A confirma", "A confirm" },
        { "CASTILLO INTERIOR", "INTERIOR CASTLE" },
        { "A: entrar", "A: enter" },
        { "B: titulo", "B: title" },
        { "R: callar / L: memoria", "R: silence / L: memory" },
        { " M", " D" },
        { " LL", " KEY" },
        { " P", " MER" },
        { "D andar A golpe B luz", "D move A hit B pray" },
        { "PAUSA A seguir B titulo", "PAUSE A resume B title" },
        { "La morada se rehace.", "Dwelling remade." },
        { "Regresa al camino.", "Back to the path." },
        { "R frente al umbral.", "R faces threshold." },
        { "La piedra ensena.", "Stone teaches." },
        { "Falta la llave interior.", "Need inner key." },
        { "R: orar tres turnos.", "R: pray three turns." },
        { "Aqui pesa el silencio.", "Silence is heavy here." },
        { "Pan para el camino.", "Bread for the road." },
        { "Luz recibida.", "Light received." },
        { "Llave interior.", "Inner key." },
        { "Resolucion.", "Resolve." },
        { "El golpe no discierne.", "A blow cannot discern." },
        { "Falta luz.", "Need light." },
        { "Misericordia y luz.", "Mercy and light." },
        { "La luz discierne.", "Light discerns." },
        { "El umbral cede.", "The threshold yields." },
        { "Silencio restaurado.", "Silence restores." },
        { "Permanece.", "Remain." },
        { "Callar da luz.", "Silence gives light." },
        { "Hay ruido cerca.", "Noise is near." },
        { "La llamada va al ESTE.", "Call goes EAST." },
        { "La llamada va al OESTE.", "Call goes WEST." },
        { "La llamada va al SUR.", "Call goes SOUTH." },
        { "La llamada va al NORTE.", "Call goes NORTH." },
        { "La llamada esta aqui.", "The call is here." },
        { "Aun no hay memoria.", "No memory yet." },
        { "La puerta falsa hiere.", "False door hurts." },
        { "Esperanza levanta.", "Hope lifts you." },
        { "La bestia golpea.", "The beast strikes." },
        { "La sombra hiere.", "The shadow hurts." },
        { "El ruido apaga luz.", "Noise dims light." },
        { "No toda puerta es casa.", "Not every door is home." },
        { "LA PUERTA FALSA", "THE FALSE DOOR" },
        { " SELLOS", " SEALS" },
        { "VENTANA DE SILENCIO", "SILENCE WINDOW" },
        { "RUIDO: no ataques", "NOISE: do not attack" },
        { "R calla  B ora  A hiere", "R hush  B pray  A hurts" },
        { "Silencio verdadero.", "True silence." },
        { "Aun hay ruido.", "Noise remains." },
        { "La puerta se vacia.", "The door empties." },
        { "Orar sin oir duele.", "Pray unheard hurts." },
        { "Golpear la alimenta.", "Striking feeds it." },
        { "El ruido muerde.", "Noise bites." },
        { "Las memorias reconocen.", "Memories recognize." },
        { "Pocas memorias alumbran.", "Few memories shine." },
        { "HAS VUELTO A CASA", "YOU HAVE COME HOME" },
        { "A: otra peregrinacion", "A: another pilgrimage" },
        { "NOCHE INTERIOR", "INNER NIGHT" },
        { "Caer no es perderse.", "Falling is not lost." },
        { "A: repetir morada", "A: retry dwelling" },
        { "La fuente no se fuerza.", "The spring is not forced." },
        { "La prisa apago la lampara.", "Haste dimmed the lamp." },
        { "Nombrar la sombra ayuda.", "Name the shadow." }
    };

    const LocalizedText pilgrim_names[] = {
        { "FE", "FAITH" },
        { "ESPERANZA", "HOPE" },
        { "CARIDAD", "CHARITY" },
        { "PAZ", "PEACE" }
    };

    const LocalizedText pilgrim_rules[] = {
        { "+1 luz maxima.", "+1 max light." },
        { "Una caida te levanta.", "One fall lifts you." },
        { "La oracion pacifica.", "Prayer pacifies." },
        { "Menos ruido enemigo.", "Foes sense less." }
    };

    const LocalizedText dwelling_lore[] = {
        { "I  Conocerse abre.", "I  Knowing opens." },
        { "II La llamada orienta.", "II The call points." },
        { "III La regla sostiene.", "III The rule sustains." },
        { "IV La quietud revela.", "IV Stillness reveals." },
        { "V  Confiar es andar.", "V  Trust is walking." },
        { "VI El fuego purifica.", "VI Fire purifies." },
        { "VII La casa llama.", "VII Home calls." }
    };

    const LocalizedText dwelling_rules[] = {
        { "Aprende limites y luz.", "Learn limits and light." },
        { "R escucha la llamada.", "R hears the call." },
        { "Recursos escasos.", "Resources are scarce." },
        { "R recupera luz segura.", "R restores safe light." },
        { "Puertas falsas abundan.", "False doors abound." },
        { "Bestias guardan umbrales.", "Beasts guard thresholds." },
        { "Discierne la puerta.", "Discern the door." }
    };

    const LocalizedText memory_texts[] = {
        { "No estaba lejos.", "It was not far." },
        { "La voz no grita.", "The voice is not loud." },
        { "La regla no encierra.", "The rule does not cage." },
        { "Callar tambien anda.", "Silence also walks." },
        { "La luz basta cerca.", "Near light is enough." },
        { "El deseo se limpia.", "Desire is cleansed." },
        { "Era hondo.", "It was deep." }
    };

    const char* localize(Language language, const LocalizedText& value)
    {
        return language == Language::spanish ? value.es : value.en;
    }

    const char* text(Language language, TextId id)
    {
        return localize(language, text_table[int(id)]);
    }
}
