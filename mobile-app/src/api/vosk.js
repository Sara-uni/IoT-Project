import { registerPlugin } from "@capacitor/core";

const VoskPlugin = registerPlugin("VoskPlugin");

const startRecognition = async () => {
  try {
    await VoskPlugin.startRecognition();
    return {
      message: "Recording Started.",
      type: "info",
    };
  } catch (err) {
    return {
      message: "Error while starting the recording: " + err.message,
      type: "error",
    };
  }
};

const stopRecognition = async () => {
  try {
    await VoskPlugin.stopRecognition();
    return {
      message: "Recording stopped.",
      type: "info",
    };
  } catch (err) {
    return {
      message: "Error while stopping the recording: " + err.message,
      type: "error",
    };
  }
};

const isListening = async () => {
  return await VoskPlugin.isListening();
};

export const Vosk = {
  startRecognition,
  stopRecognition,
  isListening,
};
