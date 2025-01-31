import { CapacitorHttp } from "@capacitor/core";

const API_BASE_URL = "http://10.254.254.129:3000"; //IP ESP

const header = {
  url: API_BASE_URL,
  timeout: 5000,
  headers: {
    "Content-Type": "application/json",
  },
};

const getData = async (data) => {
  try {
    const options = header;
    options.url = `${API_BASE_URL}/data/${data}`;
    const response = await CapacitorHttp.get(options);
    return response.data;
  } catch (error) {
    console.error("Errore nella richiesta:", error);
    return { status: "error", message: "Errore nella richiesta" };
  }
};

const toggleLed = async (mode) => {
  try {
    const options = header;
    options.url = `${API_BASE_URL}/led/${mode}`;
    const response = await CapacitorHttp.get(options);
    return response.data;
  } catch (error) {
    console.error("Errore nella richiesta:", error);
    return { status: "error", message: "Errore nella richiesta" };
  }
};

const setLedColor = async (r, g, b) => {
  try {
    const options = header;
    options.url = `${API_BASE_URL}/led-color?r=${r}&g=${g}&b=${b}`;
    const response = await CapacitorHttp.get(options);
    return response.data;
  } catch (error) {
    console.error("Errore nella richiesta:", error);
    return { status: "error", message: "Errore nella richiesta" };
  }
};

const ApiService = { getData, toggleLed, setLedColor };

export default ApiService;
