/**
 * Live-dictionary simulator for SimpleLeakAnalyzer.
 * Markup: .live-sim with data-events='[{"type":"alloc","ptr":"...","size":4,"file":"main.cpp","line":7}, ...]'
 * Optional: data-caption for the heading under the controls.
 */
(function () {
  function formatLive(live) {
    const keys = Object.keys(live);
    if (keys.length === 0) return "（空）";
    return keys
      .map((k) => {
        const e = live[k];
        return k + " → " + e.size + "B @ " + e.file + ":" + e.line;
      })
      .join("\n");
  }

  function wireSim(root) {
    let events;
    try {
      events = JSON.parse(root.getAttribute("data-events") || "[]");
    } catch {
      root.textContent = "模拟器：data-events JSON 无效";
      return;
    }

    const live = Object.create(null);
    let unknown = 0;
    let i = 0;

    const stepEl = document.createElement("pre");
    stepEl.className = "sim-step";
    const liveEl = document.createElement("pre");
    liveEl.className = "sim-live";
    const resultEl = document.createElement("p");
    resultEl.className = "sim-result";

    const btn = document.createElement("button");
    btn.type = "button";
    btn.textContent = "下一步";

    const reset = document.createElement("button");
    reset.type = "button";
    reset.textContent = "重来";
    reset.style.marginLeft = "0.5rem";

    function renderIdle() {
      if (i >= events.length) {
        const leaks = Object.keys(live).length;
        stepEl.textContent = "事件已处理完。";
        resultEl.textContent =
          "结论：泄漏 " + leaks + " 块；UnknownFree " + unknown + "。";
        resultEl.className = "sim-result " + (leaks ? "bad" : "ok");
        btn.disabled = true;
      } else {
        const e = events[i];
        stepEl.textContent =
          "待处理 #" +
          (i + 1) +
          "/" +
          events.length +
          "：event=" +
          e.type +
          (e.ptr ? " ptr=" + e.ptr : "") +
          (e.leak_count != null ? " leak_count=" + e.leak_count : "");
        resultEl.textContent = "";
        resultEl.className = "sim-result";
        btn.disabled = false;
      }
      liveEl.textContent = "live 字典：\n" + formatLive(live);
    }

    function apply(e) {
      if (e.type === "alloc") {
        live[e.ptr] = {
          size: e.size,
          file: e.file,
          line: e.line,
        };
      } else if (e.type === "free") {
        if (!(e.ptr in live)) unknown++;
        else delete live[e.ptr];
      }
      // summary: ignore
    }

    btn.addEventListener("click", () => {
      if (i >= events.length) return;
      apply(events[i]);
      i++;
      renderIdle();
    });

    reset.addEventListener("click", () => {
      Object.keys(live).forEach((k) => delete live[k]);
      unknown = 0;
      i = 0;
      renderIdle();
    });

    const caption = root.getAttribute("data-caption");
    if (caption) {
      const h = document.createElement("p");
      h.innerHTML = "<strong>" + caption + "</strong>";
      root.appendChild(h);
    }
    root.appendChild(btn);
    root.appendChild(reset);
    root.appendChild(stepEl);
    root.appendChild(liveEl);
    root.appendChild(resultEl);
    renderIdle();
  }

  document.querySelectorAll(".live-sim").forEach(wireSim);
})();
