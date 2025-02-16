import { SpeechRecognition } from "@capacitor-community/speech-recognition";

export const requestPermission = async () => {
  const available = await SpeechRecognition.available();
  if (!available) {
    console.log("Il riconoscimento vocale non è disponibile");
    return;
  }

  await SpeechRecognition.requestPermissions();
};

export const startListening = async () => {
  try {
    const result = await SpeechRecognition.start({
      language: "it-IT",
      maxResults: 1,
      prompt: "Parla ora...",
      partialResults: false,
    });
    if (result.matches && result.matches.length > 0) return result.matches[0];
  } catch (error) {
    console.error("Errore nel riconoscimento vocale", error);
  } finally {
    stopListening();
  }
  return null;
};

export const stopListening = async () => {
  console.log("Stop riconoscimento vocale");
  await SpeechRecognition.stop();
  await SpeechRecognition.removeAllListeners();
};
