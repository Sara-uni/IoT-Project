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
    options.url = `${API_BASE_URL}/${data}`;
    const response = await CapacitorHttp.get(options);
    return response.data;
  } catch (error) {
    console.error("Errore nella richiesta:", error);
  }
};

const toggleLed = async (mode) => {
  try {
    // const response = await api.get(`/led?mode=${mode}`);
    // return response.data;
  } catch (error) {
    console.error("Errore nella richiesta:", error);
  }
};

const setLedColor = async (color) => {
  try {
    //const response = await api.get(`/led?color=${color}`);
    //return response.data;
  } catch (error) {
    console.error("Errore nella richiesta:", error);
  }
};

const ApiService = { getData, toggleLed, setLedColor };

export default ApiService;
