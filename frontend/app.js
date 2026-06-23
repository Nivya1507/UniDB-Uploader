// State Management
let currentTab = '#dashboard';
let stats = { studentCount: 0, professorCount: 0, adminCount: 0, averageGpa: 0, totalSalary: 0 };

// Initialize App
document.addEventListener('DOMContentLoaded', () => {
    initRouter();
    loadDashboardStats();
    setupEventListeners();
});

// Basic Hash Router
function initRouter() {
    const handleRoute = () => {
        const hash = window.location.hash || '#dashboard';
        currentTab = hash;
        
        // Update nav active states
        document.querySelectorAll('.nav-item').forEach(el => {
            el.classList.remove('active');
            if (el.getAttribute('href') === hash) {
                el.classList.add('active');
            }
        });

        // Hide all views, then show current view
        document.querySelectorAll('.content-view').forEach(view => {
            view.classList.remove('active');
        });

        if (hash === '#dashboard') {
            document.getElementById('view-dashboard').classList.add('active');
            loadDashboardStats();
        } else if (hash === '#students') {
            document.getElementById('view-students').classList.add('active');
            loadStudents();
        } else if (hash === '#professors') {
            document.getElementById('view-professors').classList.add('active');
            loadProfessors();
        } else if (hash === '#admins') {
            document.getElementById('view-admins').classList.add('active');
            loadAdmins();
        }
        
        // Clear search input when switching tabs
        if (!hash.startsWith('#search')) {
            document.getElementById('global-search').value = '';
        }
    };

    window.addEventListener('hashchange', handleRoute);
    handleRoute(); // Call once on start
}

// Setup Event Listeners
function setupEventListeners() {
    const modal = document.getElementById('member-modal');
    const roleSelect = document.getElementById('member-role');
    const closeBtn = document.getElementById('btn-modal-close');
    const cancelBtn = document.getElementById('btn-modal-cancel');
    const form = document.getElementById('member-form');
    
    // Add Member Button triggers modal
    document.getElementById('btn-add-member').addEventListener('click', () => {
        openModal('add');
    });

    // Close Modals
    closeBtn.addEventListener('click', closeModal);
    cancelBtn.addEventListener('click', closeModal);
    modal.addEventListener('click', (e) => {
        if (e.target === modal) closeModal();
    });

    // Form role field toggling
    roleSelect.addEventListener('change', () => {
        toggleRoleFields(roleSelect.value);
    });

    // Handle Form Submit
    form.addEventListener('submit', handleFormSubmit);

    // Global Polymorphic Search
    const searchInput = document.getElementById('global-search');
    searchInput.addEventListener('input', (e) => {
        const query = e.target.value.trim();
        if (query.length > 0) {
            // Force search view
            document.querySelectorAll('.content-view').forEach(view => view.classList.remove('active'));
            document.getElementById('view-search').classList.add('active');
            document.getElementById('search-results-summary').textContent = `Showing matches for "${query}"`;
            performSearch(query);
        } else {
            // Revert back to the tab based on hash
            window.location.hash = currentTab;
            initRouter();
        }
    });
}

// Open/Close Modal
function openModal(mode, role = 'student', data = null) {
    const modal = document.getElementById('member-modal');
    const form = document.getElementById('member-form');
    const formMode = document.getElementById('form-mode');
    const modalTitle = document.getElementById('modal-title');
    const roleSelect = document.getElementById('member-role');
    const idInput = document.getElementById('member-id');

    form.reset();
    formMode.value = mode;
    modal.classList.add('active');

    if (mode === 'add') {
        modalTitle.textContent = "Add New Member";
        roleSelect.disabled = false;
        idInput.disabled = false;
        roleSelect.value = "student";
        toggleRoleFields("student");
    } else {
        modalTitle.textContent = `Edit Member Details`;
        roleSelect.value = role;
        roleSelect.disabled = true;
        idInput.value = data.id;
        idInput.disabled = true; // Cannot edit unique ID
        
        toggleRoleFields(role);

        // Prepopulate basic info
        document.getElementById('member-name').value = data.name;
        document.getElementById('member-email').value = data.email;
        document.getElementById('member-dept').value = data.department;

        // Prepopulate role specific info
        if (role === 'student') {
            document.getElementById('student-gpa').value = data.gpa;
            document.getElementById('student-major').value = data.major;
            document.getElementById('student-courses').value = data.courses ? data.courses.join(';') : '';
        } else if (role === 'professor') {
            document.getElementById('professor-salary').value = data.salary;
            document.getElementById('professor-room').value = data.room;
            document.getElementById('professor-courses').value = data.courses ? data.courses.join(';') : '';
        } else if (role === 'admin') {
            document.getElementById('admin-title').value = data.title;
            document.getElementById('admin-clearance').value = data.accessLevel;
        }
    }
}

function closeModal() {
    document.getElementById('member-modal').classList.remove('active');
}

function toggleRoleFields(role) {
    document.getElementById('student-fields').style.display = role === 'student' ? 'block' : 'none';
    document.getElementById('professor-fields').style.display = role === 'professor' ? 'block' : 'none';
    document.getElementById('admin-fields').style.display = role === 'admin' ? 'block' : 'none';
}

// Fetch API Helper
async function apiRequest(url, method = 'GET', body = null) {
    const options = {
        method,
        headers: {
            'Content-Type': 'application/json'
        }
    };
    if (body) {
        options.body = JSON.stringify(body);
    }
    try {
        const response = await fetch(url, options);
        const text = await response.text();
        // cpp-httplib might return empty bodies on success
        return text ? JSON.parse(text) : {};
    } catch (err) {
        console.error("API error:", err);
        showToast("Server Connection Failed", "error");
        throw err;
    }
}

// Load stats
async function loadDashboardStats() {
    try {
        const data = await apiRequest('/api/stats');
        stats = data;
        document.getElementById('stat-students').textContent = data.studentCount;
        document.getElementById('stat-professors').textContent = data.professorCount;
        document.getElementById('stat-admins').textContent = data.adminCount;
        document.getElementById('stat-gpa').textContent = data.averageGpa.toFixed(2);
    } catch (err) {
        console.error("Failed to load stats");
    }
}

// Load Students
async function loadStudents() {
    const tbody = document.querySelector('#students-table tbody');
    tbody.innerHTML = '<tr><td colspan="8" class="loading">Loading Students...</td></tr>';
    try {
        const list = await apiRequest('/api/students');
        tbody.innerHTML = '';
        if (list.length === 0) {
            tbody.innerHTML = '<tr><td colspan="8" style="text-align: center;">No student records found.</td></tr>';
            return;
        }
        list.forEach(student => {
            const tr = document.createElement('tr');
            
            // Format course tags
            const coursesTags = student.courses.map(c => `<span class="tag">${c}</span>`).join(' ');
            
            tr.innerHTML = `
                <td><strong>${student.id}</strong></td>
                <td>${student.name}</td>
                <td>${student.email}</td>
                <td>${student.department}</td>
                <td><span class="gpa-badge">${student.gpa.toFixed(2)}</span></td>
                <td>${student.major}</td>
                <td><div class="tag-list">${coursesTags}</div></td>
                <td>
                    <button class="btn-action edit" onclick="editMember('student', '${student.id}')">✏️</button>
                    <button class="btn-action delete" onclick="deleteMember('student', '${student.id}')">🗑️</button>
                </td>
            `;
            tbody.appendChild(tr);
        });
    } catch (err) {
        tbody.innerHTML = '<tr><td colspan="8" style="color:var(--accent-danger)">Error loading students.</td></tr>';
    }
}

// Load Professors
async function loadProfessors() {
    const tbody = document.querySelector('#professors-table tbody');
    tbody.innerHTML = '<tr><td colspan="8" class="loading">Loading Professors...</td></tr>';
    try {
        const list = await apiRequest('/api/professors');
        tbody.innerHTML = '';
        if (list.length === 0) {
            tbody.innerHTML = '<tr><td colspan="8" style="text-align: center;">No professor records found.</td></tr>';
            return;
        }
        list.forEach(prof => {
            const tr = document.createElement('tr');
            const coursesTags = prof.courses.map(c => `<span class="tag">${c}</span>`).join(' ');
            tr.innerHTML = `
                <td><strong>${prof.id}</strong></td>
                <td>${prof.name}</td>
                <td>${prof.email}</td>
                <td>${prof.department}</td>
                <td>$${prof.salary.toLocaleString()}</td>
                <td>${prof.room}</td>
                <td><div class="tag-list">${coursesTags}</div></td>
                <td>
                    <button class="btn-action edit" onclick="editMember('professor', '${prof.id}')">✏️</button>
                    <button class="btn-action delete" onclick="deleteMember('professor', '${prof.id}')">🗑️</button>
                </td>
            `;
            tbody.appendChild(tr);
        });
    } catch (err) {
        tbody.innerHTML = '<tr><td colspan="8" style="color:var(--accent-danger)">Error loading professors.</td></tr>';
    }
}

// Load Admins
async function loadAdmins() {
    const tbody = document.querySelector('#admins-table tbody');
    tbody.innerHTML = '<tr><td colspan="7" class="loading">Loading Admins...</td></tr>';
    try {
        const list = await apiRequest('/api/admins');
        tbody.innerHTML = '';
        if (list.length === 0) {
            tbody.innerHTML = '<tr><td colspan="7" style="text-align: center;">No admin records found.</td></tr>';
            return;
        }
        list.forEach(admin => {
            const tr = document.createElement('tr');
            tr.innerHTML = `
                <td><strong>${admin.id}</strong></td>
                <td>${admin.name}</td>
                <td>${admin.email}</td>
                <td>${admin.department}</td>
                <td>${admin.title}</td>
                <td><span class="pill admin">${admin.accessLevel}</span></td>
                <td>
                    <button class="btn-action edit" onclick="editMember('admin', '${admin.id}')">✏️</button>
                    <button class="btn-action delete" onclick="deleteMember('admin', '${admin.id}')">🗑️</button>
                </td>
            `;
            tbody.appendChild(tr);
        });
    } catch (err) {
        tbody.innerHTML = '<tr><td colspan="7" style="color:var(--accent-danger)">Error loading administrators.</td></tr>';
    }
}

// Perform Polymorphic Search
async function performSearch(query) {
    const tbody = document.querySelector('#search-table tbody');
    tbody.innerHTML = '<tr><td colspan="7" class="loading">Searching...</td></tr>';
    try {
        const results = await apiRequest(`/api/search?q=${encodeURIComponent(query)}`);
        tbody.innerHTML = '';
        if (results.length === 0) {
            tbody.innerHTML = '<tr><td colspan="7" style="text-align: center;">No matching members found.</td></tr>';
            return;
        }
        results.forEach(item => {
            const tr = document.createElement('tr');
            
            // Format dynamic details column based on role
            let details = '';
            if (item.role === 'student') {
                details = `GPA: ${item.gpa.toFixed(2)} | Major: ${item.major}`;
            } else if (item.role === 'professor') {
                details = `Office: ${item.room} | Salary: $${item.salary.toLocaleString()}`;
            } else if (item.role === 'admin') {
                details = `Title: ${item.title} | Clearance: ${item.accessLevel}`;
            }
            
            tr.innerHTML = `
                <td><span class="pill ${item.role}">${item.role}</span></td>
                <td><strong>${item.id}</strong></td>
                <td>${item.name}</td>
                <td>${item.email}</td>
                <td>${item.department}</td>
                <td><span style="font-size: 0.85rem; color: var(--text-secondary);">${details}</span></td>
                <td>
                    <button class="btn-action edit" onclick="editMember('${item.role}', '${item.id}')">✏️</button>
                    <button class="btn-action delete" onclick="deleteMember('${item.role}', '${item.id}')">🗑️</button>
                </td>
            `;
            tbody.appendChild(tr);
        });
    } catch (err) {
        tbody.innerHTML = '<tr><td colspan="7" style="color:var(--accent-danger)">Error executing search.</td></tr>';
    }
}

// Handle Form Submission (Add or Edit)
async function handleFormSubmit(e) {
    e.preventDefault();
    const mode = document.getElementById('form-mode').value;
    const role = document.getElementById('member-role').value;
    const id = document.getElementById('member-id').value.trim();
    
    // Read common fields
    const name = document.getElementById('member-name').value.trim();
    const email = document.getElementById('member-email').value.trim();
    const department = document.getElementById('member-dept').value.trim();

    // Prepare JSON payload
    let payload = { id, name, email, department };
    
    if (role === 'student') {
        payload.gpa = parseFloat(document.getElementById('student-gpa').value) || 0.0;
        payload.major = document.getElementById('student-major').value.trim();
        payload.courses = document.getElementById('student-courses').value.trim();
    } else if (role === 'professor') {
        payload.salary = parseFloat(document.getElementById('professor-salary').value) || 0.0;
        payload.room = document.getElementById('professor-room').value.trim();
        payload.courses = document.getElementById('professor-courses').value.trim();
    } else if (role === 'admin') {
        payload.title = document.getElementById('admin-title').value.trim();
        payload.accessLevel = document.getElementById('admin-clearance').value;
    }

    let url = `/api/${role}s`;
    let method = 'POST';

    if (mode === 'edit') {
        url = `/api/${role}s/${id}`;
        method = 'PUT';
    }

    try {
        const response = await apiRequest(url, method, payload);
        if (response.error) {
            showToast(response.error, "error");
        } else {
            showToast(`Member ${mode === 'add' ? 'created' : 'updated'} successfully!`, "success");
            closeModal();
            // Refresh view
            if (window.location.hash === `#${role}s`) {
                if (role === 'student') loadStudents();
                else if (role === 'professor') loadProfessors();
                else if (role === 'admin') loadAdmins();
            } else {
                window.location.hash = `#${role}s`;
            }
            loadDashboardStats();
        }
    } catch (err) {
        showToast("Error processing request", "error");
    }
}

// Edit Member Trigger
async function editMember(role, id) {
    try {
        const list = await apiRequest(`/api/${role}s`);
        const item = list.find(x => x.id === id);
        if (item) {
            openModal('edit', role, item);
        } else {
            showToast("Member details could not be found", "error");
        }
    } catch (err) {
        showToast("Failed to fetch member details", "error");
    }
}

// Delete Member Trigger
async function deleteMember(role, id) {
    if (confirm(`Are you sure you want to permanently delete member ${id}?`)) {
        try {
            const response = await apiRequest(`/api/${role}s/${id}`, 'DELETE');
            if (response.error) {
                showToast(response.error, "error");
            } else {
                showToast("Member successfully deleted", "success");
                
                // Refresh list
                if (role === 'student') loadStudents();
                else if (role === 'professor') loadProfessors();
                else if (role === 'admin') loadAdmins();
                
                loadDashboardStats();
                
                // If on search, refresh search results
                const searchQuery = document.getElementById('global-search').value.trim();
                if (searchQuery.length > 0) {
                    performSearch(searchQuery);
                }
            }
        } catch (err) {
            showToast("Error deleting member", "error");
        }
    }
}

// Toast System
function showToast(message, type = 'success') {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = `toast ${type}`;
    toast.textContent = message;
    
    container.appendChild(toast);
    
    // Animate out and remove
    setTimeout(() => {
        toast.classList.add('fade-out');
        toast.addEventListener('animationend', () => {
            toast.remove();
        });
    }, 4000);
}

// Global functions attached to window for inline onclick triggers
window.editMember = editMember;
window.deleteMember = deleteMember;
