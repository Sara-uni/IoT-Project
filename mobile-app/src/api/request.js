import { CapacitorHttp } from "@capacitor/core";

const storedIp = localStorage.getItem("ip");
let API_BASE_URL = "http://" + storedIp || "http://10.254.254.109";

const header = {
  url: API_BASE_URL,
  timeout: 5000,
  headers: {
    "Content-Type": "application/json",
  },
};

const setServerIP = () => {
  const storedIp = localStorage.getItem("ip");
  API_BASE_URL = "http://" + storedIp || "http://10.254.254.109";
};

const getData = async (data) => {
  try {
    const options = header;
    options.url = `${API_BASE_URL}/${data}`;
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

const getLedStatus = async () => {
  try {
    const options = header;
    options.url = `${API_BASE_URL}/led-status`;
    const response = await CapacitorHttp.get(options);
    return response.data;
  } catch (error) {
    console.error("Errore nella richiesta:", error);
    return { status: "error", message: "Errore nella richiesta" };
  }
};

const sendCommand = async (command) => {
  try {
    const options = header;
    options.url = `${API_BASE_URL}/command`;
    options.data = { command };
    const response = await CapacitorHttp.post(options);
    return response.data;
  } catch (error) {
    console.error("Errore nella richiesta:", error);
    return { status: "error", message: "Errore nella richiesta" };
  }
};
const ApiService = {
  setServerIP,
  getData,
  toggleLed,
  setLedColor,
  getLedStatus,
  sendCommand,
};

export default ApiService;
