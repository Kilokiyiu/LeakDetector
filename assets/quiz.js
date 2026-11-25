(function () {
  function wireQuiz(root) {
    const feedback = root.querySelector(".feedback");
    const buttons = root.querySelectorAll("button[data-correct]");
    buttons.forEach((btn) => {
      btn.addEventListener("click", () => {
        const correct = btn.getAttribute("data-correct") === "true";
        buttons.forEach((b) => (b.disabled = true));
        feedback.textContent = correct
          ? root.dataset.ok || "对了。"
          : root.dataset.bad || "再想一下。";
        feedback.className = "feedback " + (correct ? "ok" : "bad");
      });
    });
  }

  document.querySelectorAll(".quiz").forEach(wireQuiz);
})();
