# This files contains your custom actions which can be used to run
# custom Python code.
#
# See this guide on how to implement these action:
# https://rasa.com/docs/rasa/custom-actions


# This is a simple example for a custom action which utters "Hello World!"

from typing import Any, Text, Dict, List

from rasa_sdk import Action, Tracker
from rasa_sdk.executor import CollectingDispatcher


class ActionSendTemperature(Action):

    def name(self) -> Text:
        return "send_temperature"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:

        dispatcher.utter_message(text="Ecco la temperatura!")

        return []


class ActionSendLight(Action):

    def name(self) -> Text:
        return "send_light"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:

        dispatcher.utter_message(text="Ecco la luce!")

        return []


class ActionSendNoise(Action):

    def name(self) -> Text:
        return "send_noise"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:

        dispatcher.utter_message(text="Ecco il rumore!")

        return []


class ActionSendRepeat(Action):

    def name(self) -> Text:
        return "send_repeat"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:

        dispatcher.utter_message(text="Non ho capito, puoi ripetere?")

        return []


class ActionTurnOnLed(Action):
    def name(self) -> Text:
        return "turn_LED_on"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List:
        dispatcher.utter_message(text="Sto accendendo il LED.")
        return []


class ActionTurnOffLed(Action):
    def name(self) -> Text:
        return "turn_LED_off"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List:
        dispatcher.utter_message(text="Sto spegnendo il LED.")
        return []


class ActionChangeLedColor(Action):
    def name(self) -> Text:
        return "change_LED_color"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List:

        # Cerca l'entità 'color' nel messaggio dell'utente
        entities = tracker.latest_message.get("entities", [])
        color = None

        # Cerca l'entità 'color' nelle entità riconosciute
        for entity in entities:
            if entity.get("entity") == "color":
                color = entity.get("value")
                break

        # Controlla se il colore esiste
        if color and color in color_map:
            led_color = color_map[color]  # Ottieni i valori RGB del colore
            # Aggiungi qui la logica per cambiare effettivamente il colore del LED
            dispatcher.utter_message(text=f"Sto cambiando il colore del LED a {color}.")
        else:
            dispatcher.utter_message(text="Non ho capito quale colore impostare o non è un colore valido. Puoi ripetere?")

        return []


color_map = {
    "red":      [255, 0, 0],
    "green":    [0, 255, 0],
    "blue":     [0, 0, 255],
    "yellow":   [255, 255, 0],
    "purple":   [255, 0, 255],
    "cyan":     [0, 255, 255],
    "white":    [255, 255, 255],
}