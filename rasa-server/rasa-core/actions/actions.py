# This files contains your custom actions which can be used to run
# custom Python code.
#
# See this guide on how to implement these action:
# https://rasa.com/docs/rasa/custom-actions

from typing import Any, Text, Dict, List

from rasa_sdk import Action, Tracker
from rasa_sdk.executor import CollectingDispatcher


class ActionSendTemperature(Action):

    def name(self) -> Text:
        return "send_temperature"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:

        dispatcher.utter_message(text="GET_TEMP")

        return []


class ActionSendLight(Action):

    def name(self) -> Text:
        return "send_light"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:

        dispatcher.utter_message(text="GET_LIGHT")

        return []


class ActionSendNoise(Action):

    def name(self) -> Text:
        return "send_noise"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:

        dispatcher.utter_message(text="GET_NOISE")

        return []


class ActionSendRepeat(Action):

    def name(self) -> Text:
        return "send_repeat"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:

        dispatcher.utter_message(text="NOTHING")

        return []


class ActionTurnOnLed(Action):
    def name(self) -> Text:
        return "turn_LED_on"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List:
        dispatcher.utter_message(text="LED_ON")
        return []


class ActionTurnOffLed(Action):
    def name(self) -> Text:
        return "turn_LED_off"

    def run(self, dispatcher: CollectingDispatcher,
            tracker: Tracker,
            domain: Dict[Text, Any]) -> List:
        dispatcher.utter_message(text="LED_OFF")
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
            dispatcher.utter_message(text=f"SET_COLOR({led_color[0]}, {led_color[1]}, {led_color[2]})")
        else:
            dispatcher.utter_message(text="NOTHING")

        return []


color_map = {
    "red":      [255, 0, 0],
    "green":    [0, 255, 0],
    "blue":     [0, 0, 255],
    "yellow":   [255, 255, 0],
    "purple":   [255, 0, 255],
    "cyan":     [0, 255, 255],
    "light blue":     [0, 255, 255],
    "white":    [255, 255, 255],
    "black":    [0, 0, 0],
    "orange":   [255, 165, 0],
    "pink":     [255, 192, 203],
    "brown":    [139, 69, 19],
    "gray":     [128, 128, 128]
}