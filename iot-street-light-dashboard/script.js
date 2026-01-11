console.log("Dashboard loaded");

// -----------------------------
// MQTT Connection
// -----------------------------
const client = mqtt.connect("wss://broker.hivemq.com:8884/mqtt");

const maxPoints = 20;
let lastUpdateTime = 0;

// -----------------------------
// Data storage
// -----------------------------
const labels = [];
const ldrData = [];
const rainData = [];
const ldrLedData = []; // NEW for LED LDR

// Latest sensor values
let latestLdr = 0;
let latestRain = 0;
let latestLedFault = 0;
let latestLdrLed = 0;

// -----------------------------
// Chart creation
// -----------------------------
function createLineChart(ctx, label, data, stepped = false) {
    return new Chart(ctx, {
        type: "line",
        data: {
            labels: labels,
            datasets: [{
                label: label,
                data: data,
                borderWidth: 2,
                stepped: stepped,
                tension: 0.2
            }]
        },
        options: {
            animation: false,
            responsive: true,
            scales: {
                y: { beginAtZero: true }
            }
        }
    });
}

// Create charts
const ldrChart = createLineChart(
    document.getElementById("ldrChart"),
    "Ambient LDR",
    ldrData
);

const rainChart = createLineChart(
    document.getElementById("rainChart"),
    "Rain Status (1=Yes)",
    rainData,
    true
);

// NEW chart: Ambient LDR vs LED LDR
const ldrVsLedChart = new Chart(
    document.getElementById("ldrVsLedChart"),
    {
        type: "line",
        data: {
            labels: labels,
            datasets: [
                {
                    label: "Ambient LDR",
                    data: ldrData,
                    borderColor: "blue",
                    borderWidth: 2,
                    fill: false,
                    tension: 0.2
                },
                {
                    label: "LED LDR",
                    data: ldrLedData,
                    borderColor: "orange",
                    borderWidth: 2,
                    fill: false,
                    tension: 0.2
                }
            ]
        },
        options: {
            responsive: true,
            animation: false,
            scales: {
                y: { beginAtZero: true }
            }
        }
    }
);

// -----------------------------
// MQTT Events
// -----------------------------
client.on("connect", () => {
    console.log("✅ Connected to MQTT broker");

    client.subscribe("streetlight/ldr_ambient");
    client.subscribe("streetlight/ldr_led");
    client.subscribe("streetlight/rain");
    client.subscribe("streetlight/led_fault");
});

client.on("message", (topic, message) => {
    const value = Number(message.toString());
    const now = Date.now();

    // -----------------------------
    // Sensor updates
    // -----------------------------
    if (topic === "streetlight/ldr_ambient") {
        latestLdr = value;
        ldrData.push(value);
        if (ldrData.length > maxPoints) ldrData.shift();
        document.getElementById("ambientLdrValue").innerText = value;
    }

    if (topic === "streetlight/ldr_led") {
        latestLdrLed = value;
        ldrLedData.push(value); // NEW
        if (ldrLedData.length > maxPoints) ldrLedData.shift();
        document.getElementById("ledLdrValue").innerText = value;
    }

    if (topic === "streetlight/rain") {
        latestRain = value;
        rainData.push(value);
        if (rainData.length > maxPoints) rainData.shift();
    }

    if (topic === "streetlight/led_fault") {
        latestLedFault = value;
    }

    // -----------------------------
    // Time labels
    // -----------------------------
    if (now - lastUpdateTime > 2000) {
        labels.push(new Date().toLocaleTimeString());
        lastUpdateTime = now;
        if (labels.length > maxPoints) labels.shift();
    }

    // -----------------------------
    // Status cards
    // -----------------------------
    document.getElementById("lightCondition").innerText =
        latestLdr > 3000 ? "Dark" : "Bright";

    document.getElementById("rainCondition").innerText =
        latestRain === 1 ? "Raining" : "No Rain";

    document.getElementById("streetLightCondition").innerText =
        (latestLdr > 3000 || latestRain === 1) ? "ON" : "OFF";

    document.getElementById("ledFaultStatus").innerText =
        latestLedFault === 1 ? "BROKEN ❌" : "NORMAL ✅";

    // -----------------------------
    // Update charts
    // -----------------------------
    ldrChart.update();
    rainChart.update();
    ldrVsLedChart.update(); // NEW

    console.log(topic, value);
});
